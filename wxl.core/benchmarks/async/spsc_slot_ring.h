#pragma once

// A ring of slots in the manner of the LMAX Disruptor, single producer and single consumer,
// kept here for comparison only: the contender of a size chosen in advance against queues
// that grow and recycle their blocks. It is not part of the library and nothing outside this
// benchmark uses it.

#include <cassert>
#include <cstddef>

import std;
import wxl.core;

namespace {

// The alignas members below are meant to cost padding: that is what keeps the counters on
// lines of their own. C4324 reports the padding at level 4, and for a template it fires where
// the class is laid out, so it is silenced here, where the padding is known to be the point.
#pragma warning(push)
#pragma warning(disable : 4324)  // structure was padded due to alignment specifier

/// Bounded queue for exactly one writing and one reading thread: one array of `capacity`
/// slots allocated up front, `capacity` a power of two so that a position becomes a slot by
/// a mask. Neither side allocates, and the ring never grows: a writer that finds it full
/// spins until the reader has made room.
///
/// The two sides share two counters and nothing else, each on a line of its own: how far
/// the writer has published and how far the reader has consumed. Publishing is a release
/// store, looking is an acquire load, and neither is a read-modify-write. Each side keeps a
/// copy of the other's counter and reads the real one only when the copy runs out -- the
/// writer when the ring looks full, the reader when it looks empty.
///
/// The reader takes everything the writer had published as of its last look as one batch
/// and publishes its own counter once that batch is drained, as a Disruptor's batch handler
/// moves its sequence after the batch rather than after each event. It publishes in the
/// read that takes the batch's last element, so a writer waiting for room is never left
/// waiting on a reader that has stopped calling.
///
/// Every slot is written once when the ring is made: the pages are touched before anything
/// is measured, as the other contenders' preallocated memory is.
template <class element_t, std::size_t capacity>
class spsc_slot_ring : public wxl::core::noncopyable
{
    static_assert(capacity > 0 && (capacity & (capacity - 1)) == 0,
                  "the capacity of a slot ring is a power of two");

    static constexpr std::size_t mask = capacity - 1;
    static constexpr std::align_val_t slot_alignment{std::hardware_destructive_interference_size};

    struct slots_delete {
        void operator()(element_t* const slots) const noexcept {
            std::destroy_n(slots, capacity);
            ::operator delete(slots, slot_alignment);
        }
    };

public:
    spsc_slot_ring()
        : slots_(static_cast<element_t*>(::operator new(capacity * sizeof(element_t), slot_alignment))) {
        std::uninitialized_value_construct_n(slots_.get(), capacity);
    }

    /// Writes one element, waiting for room when the ring is full. The producing thread's call.
    void write(const element_t& element) {
        if (write_position_ - reader_seen_ == capacity) [[unlikely]] {
            do {
                reader_seen_ = consumed_.load(std::memory_order_acquire);
            } while (write_position_ - reader_seen_ == capacity);
        }

        slots_.get()[write_position_ & mask] = element;
        published_.store(++write_position_, std::memory_order_release);
    }

    /// The consuming thread's half: where it has read up to, and how far the writer had
    /// published when it last looked.
    class reader;

private:
    std::unique_ptr<element_t, slots_delete> slots_;

    // The writer's line: its published counter, and beside it the writer's own position and
    // its copy of the reader's counter, which nobody else touches.
    alignas(std::hardware_destructive_interference_size) std::atomic<std::size_t> published_{0};
    std::size_t write_position_{0};
    std::size_t reader_seen_{0};
    bool has_reader_{};

    // The reader's counter, on a line of its own: the writer reads it only when the ring looks
    // full.
    alignas(std::hardware_destructive_interference_size) std::atomic<std::size_t> consumed_{0};
};

template <class element_t, std::size_t capacity>
class spsc_slot_ring<element_t, capacity>::reader : public wxl::core::noncopyable
{
public:
    explicit reader(spsc_slot_ring& ring) noexcept : ring_(&ring) {
        assert(!ring.has_reader_ && "an spsc_slot_ring takes a single reader");
        ring.has_reader_ = true;
    }

    /// Moves the next element into `out`.
    ///
    /// @return `false` if the ring was empty as of this call.
    [[nodiscard]] bool read(element_t& out) {
        if (read_position_ == writer_seen_) {
            writer_seen_ = ring_->published_.load(std::memory_order_acquire);
            if (read_position_ == writer_seen_) return false;
        }

        out = std::move(ring_->slots_.get()[read_position_ & mask]);

        // The batch is drained: the reader's counter goes out now, with every slot of the
        // batch free for the writer again.
        if (++read_position_ == writer_seen_)
            ring_->consumed_.store(read_position_, std::memory_order_release);

        return true;
    }

private:
    spsc_slot_ring* ring_;
    std::size_t read_position_{0};
    std::size_t writer_seen_{0};  // the end of the batch: the writer's counter as of the last look
};

#pragma warning(pop)

}  // namespace
