#pragma once

// The ring of blocks wxl.async's spsc_queue used to be, kept here for comparison only: the
// contender the queue of today is measured against, a ring that never recycles and grows when
// it is full. It is not part of the library and nothing outside this benchmark uses it.
//
// The algorithm is that earlier spsc_queue's, text and all, under a name of its own.

#include <cassert>
#include <cstddef>

import std;
import wxl.core;

namespace {

// The alignas members below are meant to cost padding: that is what keeps the blocks and the
// cursor on lines of their own. C4324 reports the padding at level 4, and for a template it
// fires where the class is laid out, so it is silenced here, where the padding is known to be
// the point.
#pragma warning(push)
#pragma warning(disable : 4324)  // structure was padded due to alignment specifier

/// Unbounded queue for exactly one writing and one reading thread, made of blocks of
/// `block_size` elements linked into a ring. Neither side ever waits: the writer fills the
/// block it owns and steps to the next one, the reader walks behind it around the same
/// ring, and a queue that has reached its working size stops allocating altogether.
///
/// **Recycling is not an operation.** The block the writer needs next is the one already
/// linked after the block it has just filled, so there is no free list to put a drained
/// block into and no link to rewrite -- `next_` is written once, when a block is spliced
/// into the ring, and never again. The whole protocol between the threads is a single
/// pointer: the reader publishes the block it is working on, and the writer takes the next
/// one whenever that is not the block the reader is sitting on. When it is, the ring is
/// full and grows by one block spliced in behind the writer.
///
/// **The block the writer may not take is the point of the design, not its price.** It is
/// what keeps the two threads arbitrating at block granularity: were the writer allowed
/// into the reader's block, the two would have to compare positions inside it, which means
/// the reader publishing its index on every element read and the writer loading it on every
/// element written. Data availability travels the other way and stays per element, because
/// that direction is one-way and asks nobody for permission.
///
/// **No locked instruction anywhere on the running path.** Every value shared here has
/// exactly one writer -- the writer's position in a block, and the reader's
/// cursor -- so a release store and an acquire load carry all the ordering there is to
/// carry, and neither of them is a read-modify-write. No cache line is ever taken
/// exclusively for a CAS; on x86-64 the whole machinery is plain moves.
///
/// The queue **is** the writer: `write()` and `emplace()` belong to the producing thread
/// and the cursor they advance is a field of the queue itself, so a producer that keeps the
/// queue by value on its stack reaches its own state without a hop. The consumer's state is
/// the same kind of object one level down -- a `reader` built from the queue, by value on
/// the consuming thread's stack. There is one of each, and the second `reader` is an assert.
///
/// Teardown happens with the traffic stopped: the reader is destroyed once the writing
/// thread is done with the queue, and the queue after the reader. Every block ever
/// allocated is on the ring, and the queue frees all of them.
///
/// There is no signalling here on purpose. A consumer with nothing to read has to be woken
/// by something the caller owns -- an event next to the queue, the way `shared_buffer`
/// does it -- never by re-reading the queue on a timer.
template <class element_t, size_t block_size>
class spsc_block_ring : public wxl::core::noncopyable
{
    static_assert(block_size > 0, "a block has to hold at least one element");

    static constexpr size_t item_alignment =
        std::max(alignof(element_t), std::hardware_destructive_interference_size);

    /// One chunk of the queue: storage for `block_size` elements, how far into it the
    /// writer has got, and the link to the block that follows it around the ring.
    struct alignas(std::hardware_destructive_interference_size) block {
        std::atomic<size_t> index_{};

        // Written once, by the writer, while the block is still unpublished, and never
        // again -- so the reader, which only follows the link once it has acquired a full
        // index_, needs no atomic here. It rides the same release store as the elements.
        block* next_{};

        element_t* item(size_t index) noexcept {
            return reinterpret_cast<element_t*>(storage_ + index * sizeof(element_t));
        }

        // The writer keeps storing into this array while the reader reads out of it, so the
        // alignment starts it on a line of its own rather than on the one carrying index_
        // and next_, which the reader is reading at the same time.
        alignas(item_alignment) std::byte storage_[block_size * sizeof(element_t)];
    };

public:
    using element_type = element_t;

    static constexpr size_t elements_per_block = block_size;

    /// The smallest ring there is: one block linked to itself. It grows on the first turn,
    /// since the writer may never take the block the reader is on.
    spsc_block_ring() : write_cursor_(new block), reader_cursor_(write_cursor_) {
        write_cursor_->next_ = write_cursor_;
    }

    ~spsc_block_ring();

    /// Builds one element in place, then publishes it. The producing thread's call.
    template <class... t_args>
        requires std::constructible_from<element_t, t_args...>
    void emplace(t_args&&... args) {
        block* const current = write_cursor_;
        std::construct_at(current->item(write_index_), std::forward<t_args>(args)...);

        if (++write_index_ == block_size) [[unlikely]] {
            block* const next = take_block();
            // Everything take_block() touched is sequenced before this store, so a reader
            // that finds a full block finds its successor linked and emptied behind it.
            current->index_.store(block_size, std::memory_order_release);
            write_cursor_ = next;
            write_index_ = 0;
        } else {
            current->index_.store(write_index_, std::memory_order_release);
        }
    }

    void write(const element_t& element) { emplace(element); }

    void write(element_t&& element) { emplace(std::move(element)); }

    /// The consuming thread's half: where it has read up to, and how far the writer had
    /// published when it last looked.
    class reader;

private:
    /// The block after the one just filled, which the reader has already left behind -- it
    /// needs no relinking, it is where it has to be. If the reader is still sitting on it,
    /// the ring is full and takes one more block in that place.
    block* take_block() {
        block* const next = write_cursor_->next_;

        if (next != reader_cursor_.load(std::memory_order_acquire)) [[likely]] {
            next->index_.store(0, std::memory_order_relaxed);
            return next;
        }

        block* const grown = new block;
        ++allocated_blocks_;
        grown->next_ = next;
        write_cursor_->next_ = grown;
        return grown;
    }

    block* write_cursor_;  // the block being filled, and how far into it
    size_t write_index_{};
    size_t allocated_blocks_{1};  // every block ever created, counted down on teardown
    bool has_reader_{};

    // The whole of what the reader tells the writer, and the only line the two threads
    // share: the block the reader is working on. Everything ahead of it around the ring is
    // the writer's to fill.
    alignas(std::hardware_destructive_interference_size) std::atomic<block*> reader_cursor_;
};

template <class element_t, size_t block_size>
spsc_block_ring<element_t, block_size>::~spsc_block_ring() {
    // Opening the ring behind the writer turns it into an ordinary chain ending at the
    // block the writer stopped on, which is every block that was ever allocated.
    block* current = write_cursor_->next_;
    write_cursor_->next_ = nullptr;

    // A queue that never had a reader still holds unread elements, and with nothing ever
    // recycled every block on the ring holds exactly as many as its index says. One that
    // had a reader had them destroyed by that reader's own destructor.
    while (current) {
        block* const next = current->next_;

        if (!has_reader_)
            std::destroy_n(current->item(0), current->index_.load(std::memory_order_relaxed));

        delete current;
        --allocated_blocks_;
        current = next;
    }

    assert(allocated_blocks_ == 0 && "a reader outlived the spsc_block_ring it was attached to");
}

template <class element_t, size_t block_size>
class spsc_block_ring<element_t, block_size>::reader : public wxl::core::noncopyable
{
public:
    explicit reader(spsc_block_ring& queue) noexcept
        : read_cursor_(queue.reader_cursor_.load(std::memory_order_relaxed)), queue_(&queue) {
        assert(!queue.has_reader_ && "an spsc_block_ring takes a single reader");
        queue.has_reader_ = true;
    }

    /// Destroys whatever was left unread; the blocks themselves stay on the ring for the
    /// queue to free. With the writing thread already done, the run from the cursor to the
    /// block the writer stopped on is exactly what still holds elements -- the free part of
    /// the ring beyond it keeps indices from an earlier life, which is why the walk ends
    /// there rather than on one of them.
    ~reader() {
        block* const last = queue_->write_cursor_;

        for (block* current = read_cursor_;; current = current->next_) {
            const size_t write_index = current->index_.load(std::memory_order_acquire);
            std::destroy_n(current->item(read_index_), write_index - read_index_);
            read_index_ = 0;

            if (current == last) break;
        }
    }

    /// Moves the next element into `out`.
    ///
    /// @return `false` if the queue was empty as of this call -- which is an answer
    ///         about that moment only, since the writer may be publishing as it
    ///         returns. A caller that has to wait waits on its own event, not by
    ///         asking again.
    [[nodiscard]] bool read(element_t& out) {
        if (read_index_ == write_index_ && !advance()) return false;

        element_t* const slot = read_cursor_->item(read_index_++);
        out = std::move(*slot);
        std::destroy_at(slot);
        return true;
    }

private:
    /// Re-reads what the writer has published, stepping to the next block once this one is
    /// drained. Publishing the new cursor is what frees the old block: the release store
    /// carries the element destructors with it, so a writer that comes round to the block
    /// finds storage nobody is using. @return `false` when there is nothing to read.
    bool advance() {
        write_index_ = read_cursor_->index_.load(std::memory_order_acquire);

        if (read_index_ != write_index_) return true;
        if (write_index_ != block_size) return false;  // the writer is still filling it

        block* const next = read_cursor_->next_;  // linked once, when the ring took the block
        read_cursor_ = next;
        read_index_ = 0;
        queue_->reader_cursor_.store(next, std::memory_order_release);

        write_index_ = next->index_.load(std::memory_order_acquire);
        return write_index_ != 0;
    }

    block* read_cursor_;
    size_t read_index_{};
    size_t write_index_{};  // the writer's position in read_cursor_, as of the last look
    spsc_block_ring* queue_;     // only wanted when a block runs out
};

#pragma warning(pop)

}  // namespace
