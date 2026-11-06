// The DispatcherQueueTimer behind wxl::WaitableTimer, over the projection.
//
// The projection comes first, with every standard header it needs, as in UiThread.cpp.
#include "platform.h"

#include <winrt/Microsoft.UI.Dispatching.h>

#include "impl/conversions.h"
#include "WaitableTimer.h"
#include "impl/dispatcher.h"

namespace wxl {

namespace {

using timer_t = winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer;

/// The timer as the projection has it, lent: a projected object is the one pointer, so a call
/// through it costs no reference.
timer_t const& projected(::IInspectable* const& timer) noexcept {
    static_assert(sizeof(timer_t) == sizeof(::IInspectable*));
    return reinterpret_cast<timer_t const&>(timer);
}

/// A one-shot timer of the UI thread's queue with its interval set and nothing started.
::IInspectable* one_shot(std::chrono::milliseconds interval) {
    timer_t timer = impl::ui_queue().CreateTimer();
    timer.Interval(interval);
    timer.IsRepeating(false);
    return static_cast<::IInspectable*>(winrt::detach_abi(timer));
}

}  // namespace

namespace impl {

dispatcher_timer::~dispatcher_timer() {
    timer_t released{nullptr};
    winrt::attach_abi(released, timer_);
}

EventToken dispatcher_timer::add_tick(core::function<void()> handler) const {
    return from_winrt(
        projected(timer_).Tick([handler = std::move(handler)](auto&&, auto&&) { handler(); }));
}

void dispatcher_timer::remove(EventToken& token) const {
    projected(timer_).Tick(to_winrt(take_token(token)));
}

void dispatcher_timer::start() const { projected(timer_).Start(); }

void dispatcher_timer::stop() const { projected(timer_).Stop(); }

}  // namespace impl

WaitableTimer::WaitableTimer(std::chrono::milliseconds interval, async::cancellation_token token)
    : impl::waitable_timer<impl::dispatcher_timer>(impl::dispatcher_timer{one_shot(interval)},
                                                   std::move(token)) {}

}  // namespace wxl
