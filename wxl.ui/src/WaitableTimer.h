#pragma once

// wxl::WaitableTimer -- a wait on the thread's queue that ends when its interval is up,
// when its owner says now, or when its token is cancelled; and ends the same way each
// time, as far as the coroutine waiting can tell.
//
// For the work that follows a change after a quiet moment, and must still be done when
// there is no more waiting to be had: a draft written to disk a second and a half after
// the last keystroke, written at once when the user closes the document, and written on
// the way out whatever the timer says.
//
//     WaitableTimer timer{1500ms, source.token()};   // on the UI thread
//
//     async::task<> writes(WaitableTimer& timer, async::stop_token token) {
//         do {
//             co_await timer;
//             co_await save();
//         } while (!token.stop_requested());
//     }
//
//     timer.restart();   // a change: the interval begins again from now
//     timer.signal();    // now: the wait ends, and nothing is left to come later
//
// A tick or a signal() that comes while the coroutine is in its save is kept for the
// next wait, not lost; one that comes before a restart() is forgotten with it. The
// cancellation ends the wait on the queue's next turn, like nothing happened -- the
// coroutine saves once more and reads the token. How it is built, and what differs
// on the way down, is in impl/waitable_timer.h.

#include <chrono>
#include <utility>

#include "impl/waitable_timer.h"

// Declared, not included, as in UiThread.h: the timer is one COM pointer.
struct IInspectable;

namespace wxl {

namespace impl {

/// The DispatcherQueueTimer of the UI thread as the wait has it: the four calls, over the
/// projection in WaitableTimer.cpp. Owns one reference to the timer.
class dispatcher_timer
{
public:
    /// Adopts the reference it is given.
    explicit dispatcher_timer(::IInspectable* timer) noexcept : timer_(timer) {}

    dispatcher_timer(dispatcher_timer&& other) noexcept : timer_(std::exchange(other.timer_, nullptr)) {}
    dispatcher_timer& operator=(dispatcher_timer&&) = delete;
    ~dispatcher_timer();

    // The handler is made into a function once, here; a tick is one call through it.
    template <typename Handler>
    EventToken add(Handler&& handler) const {
        return add_tick(core::function<void()>{std::forward<Handler>(handler)});
    }

    void remove(EventToken& token) const;
    void start() const;
    void stop() const;

private:
    EventToken add_tick(core::function<void()> handler) const;

    ::IInspectable* timer_;
};

}  // namespace impl

class WaitableTimer : public impl::waitable_timer<impl::dispatcher_timer>
{
public:
    /// A one-shot timer of the UI thread's queue, made on that thread and stopped until
    /// restart(). The token, given last as everywhere in wxl, is the one every wait on the
    /// timer is registered with.
    explicit WaitableTimer(std::chrono::milliseconds interval, async::stop_token token = {});
};

}  // namespace wxl
