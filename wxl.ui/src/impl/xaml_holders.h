#pragma once

// What holds content of XAML outside XAML's own windows, and so has to let go of it while XAML
// lives: the exit closes it when the queue of XAML begins to shut down, and checks at the end
// that nothing of it is left.
//
// XAML counts only its own windows. Its loop ends with the last of them, and it takes itself
// apart inside Application::Start, in the FrameworkShutdownStarting of its queue; the queue's
// ShutdownStarting comes before, with XAML alive, and is the last moment such a holder can be
// closed the way it is meant to be. The entry point knows that moment and nothing of the holders;
// a holder knows its own and enlists them here, the first time one of them is made -- not at
// static initialisation, which an object file of a static library nobody references would never
// run.
//
// One kind of holder enlists, so one is kept: the two calls of the exit are indirect calls through
// one pointer, which points at calls that do nothing until a kind enlists. A second kind stops
// the process at enlist() rather than taking the place of the first.

#include "../core.h"

import wxl.core;

namespace wxl::impl {

class xaml_holders
{
public:
    /// The calls through which the exit reaches the holders of one kind, on the UI thread.
    struct kind {
        /// In ShutdownStarting of the queue of XAML: closes every holder still open, those
        /// opened while it runs included.
        void (*close_all)() noexcept;

        /// After the thread's loop has run dry: stops the process, with the holder named, when
        /// one is still open -- it outlived XAML, and taken apart later it would release its
        /// XAML content under a XAML that is gone.
        void (*ensure_all_closed)() noexcept;
    };

    xaml_holders() = delete;

    /// From the first holder of the kind on, the exit reaches them through \p holders, which
    /// lives as long as the process.
    static void enlist(kind const& holders) noexcept {
        if (holders_ != &none) {
            core::abort("wxl: a second kind of XAML holder enlisted; the exit keeps one");
        }
        holders_ = &holders;
    }

    static void close_all() noexcept { holders_->close_all(); }

    static void ensure_all_closed() noexcept { holders_->ensure_all_closed(); }

private:
    static void nothing() noexcept {}

    static constexpr kind none{&nothing, &nothing};

    // Constant-initialised: the exit reads it even when no holder was ever made.
    static constinit inline kind const* holders_ = &none;
};

}  // namespace wxl::impl
