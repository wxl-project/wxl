# Measurement tools, built only when WXL_BUILD_BENCHMARKS is on: each is a plain
# executable timing itself, because what is compared here is a handful of
# nanoseconds per element and no test framework should be standing in that.

wxl_add_benchmark(wxl.async.mpsc-queue-benchmark SOURCES mpsc_queue_benchmark.cpp)

target_link_libraries(wxl.async.mpsc-queue-benchmark PRIVATE wxl::core)

# spsc_queue against the ring of blocks it used to be and a ring of slots in the
# manner of a Disruptor, both kept in the headers beside it for this comparison
# only.
wxl_add_benchmark(wxl.async.spsc-queue-benchmark
    SOURCES
        spsc_queue_benchmark.cpp
        spsc_block_ring.h
        spsc_slot_ring.h
)

target_link_libraries(wxl.async.spsc-queue-benchmark PRIVATE wxl::core)

wxl_add_benchmark(wxl.async.threaded-allocator-benchmark SOURCES threaded_allocator_benchmark.cpp)

target_link_libraries(wxl.async.threaded-allocator-benchmark PRIVATE wxl::core)

wxl_add_benchmark(wxl.async.coroutine-frame-benchmark SOURCES coroutine_frame_benchmark.cpp)

target_link_libraries(wxl.async.coroutine-frame-benchmark PRIVATE wxl::core)

# A co_await of one task from another, plain and under a cancellation token.
wxl_add_benchmark(wxl.async.task-chain-benchmark SOURCES task_chain_benchmark.cpp)

target_link_libraries(wxl.async.task-chain-benchmark PRIVATE wxl::core)

# The loop end to end: what one co_await costs through it, with the real sta_loop in the
# first row and the variants beside it. synchronization.lib is WaitOnAddress, which the
# address rows stand on.
wxl_add_benchmark(wxl.async.sta-loop-benchmark SOURCES sta_loop_benchmark.cpp)

target_link_libraries(wxl.async.sta-loop-benchmark PRIVATE wxl::core synchronization)

# The wake on its own: how long a thread takes to be woken, by primitive and by how long
# it slept -- the number the loop's round trip is made of.
wxl_add_benchmark(wxl.async.thread-wake-benchmark SOURCES thread_wake_benchmark.cpp)

target_link_libraries(wxl.async.thread-wake-benchmark PRIVATE wxl::core synchronization)

# One writer and one reader through spsc_channel, mpsc_channel and the textbook ring under a
# mutex and condition variables: latency at a steady pace, by the pause between elements, and
# throughput at full speed.
wxl_add_benchmark(wxl.async.channel-one-to-one-benchmark SOURCES channel_one_to_one_benchmark.cpp)

target_link_libraries(wxl.async.channel-one-to-one-benchmark PRIVATE wxl::core)

# The loop against the native scheme: the same round trip written the WinRT way --
# resume_background() out, resume_foreground(DispatcherQueue) back, and the
# IAsyncOperation a framework call is -- beside sta_loop in its driven mode, on one
# real DispatcherQueue pumped by the STA thread. coremessaging.lib is the OS queue
# itself (CreateDispatcherQueueController); runtimeobject.lib is what the cppwinrt
# base links against. Microsoft::CppWinRT is the projection the top level of the
# tree makes (cmake/wxl_packages.cmake).
wxl_add_benchmark(wxl.async.sta-loop-vs-winrt-benchmark SOURCES sta_loop_vs_winrt_benchmark.cpp)

target_link_libraries(wxl.async.sta-loop-vs-winrt-benchmark
    PRIVATE wxl::core Microsoft::CppWinRT coremessaging.lib runtimeobject.lib)

# One read of a file, by how it is carried out: blocking on either thread, or overlapped
# and finished through the port.
wxl_add_benchmark(wxl.async.file-read-benchmark SOURCES file_read_benchmark.cpp)

target_link_libraries(wxl.async.file-read-benchmark PRIVATE wxl::core)
