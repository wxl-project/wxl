# Measurement tools, built only when WXL_BUILD_BENCHMARKS is on: each is a plain
# executable timing itself, because what is compared here is a handful of
# nanoseconds per operation and no test framework should be standing in that.

wxl_add_benchmark(wxl.core.sta-allocator-benchmark SOURCES sta_allocator_benchmark.cpp)

target_link_libraries(wxl.core.sta-allocator-benchmark PRIVATE wxl::core)

wxl_add_benchmark(wxl.core.sta-scenario-benchmark SOURCES sta_allocator_scenario_benchmark.cpp)

target_link_libraries(wxl.core.sta-scenario-benchmark PRIVATE wxl::core)

wxl_add_benchmark(wxl.core.sta-bump-only-benchmark SOURCES sta_bump_only_benchmark.cpp)

target_link_libraries(wxl.core.sta-bump-only-benchmark PRIVATE wxl::core)

wxl_add_benchmark(wxl.core.sta-get-benchmark SOURCES sta_allocator_get_benchmark.cpp)

target_link_libraries(wxl.core.sta-get-benchmark PRIVATE wxl::core)

wxl_add_benchmark(wxl.core.page-allocation-benchmark SOURCES page_allocation_benchmark.cpp)

target_link_libraries(wxl.core.page-allocation-benchmark PRIVATE wxl::core)

# The text half of the module: the transcoders against MultiByteToWideChar and
# WideCharToMultiByte. What it found is in doc/unicode.md.
wxl_add_benchmark(wxl.core.transcode-benchmark SOURCES transcode_benchmark.cpp)

target_link_libraries(wxl.core.transcode-benchmark PRIVATE wxl::core)
