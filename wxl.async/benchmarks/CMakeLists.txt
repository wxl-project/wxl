# Measurement tools, built only when WXL_BUILD_BENCHMARKS is on: each is a plain
# executable timing itself, because what is compared here is a handful of
# nanoseconds per element and no test framework should be standing in that.

wxl_add_benchmark(wxl.async.mpsc-queue-benchmark SOURCES mpsc_queue_benchmark.cpp)

target_link_libraries(wxl.async.mpsc-queue-benchmark PRIVATE wxl::async)

wxl_add_benchmark(wxl.async.spsc-queues-benchmark SOURCES spsc_queues_benchmark.cpp)

target_link_libraries(wxl.async.spsc-queues-benchmark PRIVATE wxl::async)

wxl_add_benchmark(wxl.async.threaded-allocator-benchmark SOURCES threaded_allocator_benchmark.cpp)

target_link_libraries(wxl.async.threaded-allocator-benchmark PRIVATE wxl::async)

wxl_add_benchmark(wxl.async.coroutine-frame-benchmark SOURCES coroutine_frame_benchmark.cpp)

target_link_libraries(wxl.async.coroutine-frame-benchmark PRIVATE wxl::async)

# A co_await of one task from another, plain and under a cancellation token.
wxl_add_benchmark(wxl.async.coroutine-chain-benchmark SOURCES coroutine_chain_benchmark.cpp)

target_link_libraries(wxl.async.coroutine-chain-benchmark PRIVATE wxl::async)

# The loop end to end: what one co_await costs through it, with the real sta_loop in the
# first row and the variants beside it. synchronization.lib is WaitOnAddress, which the
# address rows stand on.
wxl_add_benchmark(wxl.async.sta-loop-benchmark SOURCES sta_loop_benchmark.cpp)

target_link_libraries(wxl.async.sta-loop-benchmark PRIVATE wxl::async synchronization)

# The wake on its own: how long a thread takes to be woken, by primitive and by how long
# it slept -- the number the loop's round trip is made of.
wxl_add_benchmark(wxl.async.wake-latency-benchmark SOURCES wake_latency_benchmark.cpp)

target_link_libraries(wxl.async.wake-latency-benchmark PRIVATE wxl::async synchronization)

# One writer and one reader through spsc_channel, mpsc_channel and the textbook ring under a
# mutex and condition variables: latency at a steady pace, by the pause between elements, and
# throughput at full speed.
wxl_add_benchmark(wxl.async.channels-benchmark SOURCES channels_benchmark.cpp)

target_link_libraries(wxl.async.channels-benchmark PRIVATE wxl::async)

# The loop against the native scheme: the same round trip written the WinRT way --
# resume_background() out, resume_foreground(DispatcherQueue) back, and the
# IAsyncOperation a framework call is -- beside sta_loop in its driven mode, on one
# real DispatcherQueue pumped by the STA thread. coremessaging.lib is the OS queue
# itself (CreateDispatcherQueueController); runtimeobject.lib is what the cppwinrt
# base links against.
find_package(cppwinrt CONFIG REQUIRED)

wxl_add_benchmark(wxl.async.winrt-coroutine-benchmark SOURCES winrt_coroutine_benchmark.cpp)

target_link_libraries(wxl.async.winrt-coroutine-benchmark
    PRIVATE wxl::async Microsoft::CppWinRT coremessaging.lib runtimeobject.lib)

# One read of a file, by how it is carried out: blocking on either thread, or overlapped
# and finished through the port.
wxl_add_benchmark(wxl.async.async-file-benchmark SOURCES async_file_benchmark.cpp)

target_link_libraries(wxl.async.async-file-benchmark PRIVATE wxl::async)
