#pragma once

#include <winrt/Microsoft.UI.Dispatching.h>

#include "../core.h"
#include "../UiThread.h"

// A dispatcher: the thread work is handed over to, as the code handing it over sees it. A type is
// one -- dispatcher_like -- when it has `bool schedule(work)`: true -- the work is taken and runs
// on the dispatcher's thread; false -- the dispatcher is closed and took nothing. What hands work
// over is written against this, so that it can be tested over a fake; winrt_dispatcher is the one
// the platform's DispatcherQueue makes.
//
// Private: this header names winrt types, so only wxl's own sources include it.

namespace wxl::impl {

template <class D>
concept dispatcher_like = std::copy_constructible<D> && requires(D const& d) {
    { d.schedule(std::declval<void (*)() noexcept>()) } -> std::same_as<bool>;
};

/// The UI thread's queue, asked on it. A call from another thread, or a thread with no queue,
/// stops the process.
inline winrt::Microsoft::UI::Dispatching::DispatcherQueue ui_queue(
    std::source_location where = std::source_location::current()) {
    UiThread::ensureCurrent(where);
    auto queue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    if (!queue) core::abort("wxl: the UI thread has no dispatcher queue", where);
    return queue;
}

/// The dispatcher over a DispatcherQueue of the platform, held by value: schedule() is its
/// TryEnqueue. What TryEnqueue throws -- its call failing, the delegate not made -- goes on to the
/// caller.
class winrt_dispatcher
{
public:
    explicit winrt_dispatcher(winrt::Microsoft::UI::Dispatching::DispatcherQueue queue) noexcept
        : queue_(std::move(queue)) {}

    template <class F>
    bool schedule(F&& work) const {
        return queue_.TryEnqueue(std::forward<F>(work));
    }

private:
    winrt::Microsoft::UI::Dispatching::DispatcherQueue queue_;
};

}  // namespace wxl::impl
