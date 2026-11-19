# Measurement tools, built only when WXL_BUILD_BENCHMARKS is on: each is a plain
# executable timing itself, because what is compared here is a handful of
# nanoseconds per operation and no test framework should be standing in that.
# Each is declared by wxl_add_benchmark() and carries the manifest of wxl's
# executables.

wxl_add_benchmark(wxl.core.sta-allocator-size-grid-benchmark SOURCES sta_allocator_size_grid_benchmark.cpp)

target_link_libraries(wxl.core.sta-allocator-size-grid-benchmark PRIVATE wxl::core)

wxl_add_benchmark(wxl.core.sta-allocator-nested-containers-benchmark SOURCES sta_allocator_nested_containers_benchmark.cpp)

target_link_libraries(wxl.core.sta-allocator-nested-containers-benchmark PRIVATE wxl::core)

wxl_add_benchmark(wxl.core.sta-memory-pool-bump-benchmark SOURCES sta_memory_pool_bump_benchmark.cpp)

target_link_libraries(wxl.core.sta-memory-pool-bump-benchmark PRIVATE wxl::core)

wxl_add_benchmark(wxl.core.sta-memory-pool-alloc-path-benchmark SOURCES sta_memory_pool_alloc_path_benchmark.cpp)

target_link_libraries(wxl.core.sta-memory-pool-alloc-path-benchmark PRIVATE wxl::core)

wxl_add_benchmark(wxl.core.sta-memory-pool-page-benchmark SOURCES sta_memory_pool_page_benchmark.cpp)

target_link_libraries(wxl.core.sta-memory-pool-page-benchmark PRIVATE wxl::core)

# The text half of the module: the transcoders against MultiByteToWideChar and
# WideCharToMultiByte.
wxl_add_benchmark(wxl.core.unicode-transcode-benchmark SOURCES unicode_transcode_benchmark.cpp)

target_link_libraries(wxl.core.unicode-transcode-benchmark PRIVATE wxl::core)

# The benchmarks of the areas of the module, one directory each.
add_subdirectory(fmt)
add_subdirectory(async)
add_subdirectory(xml)
