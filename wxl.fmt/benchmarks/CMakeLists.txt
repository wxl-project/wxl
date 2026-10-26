# A measurement tool, built only when WXL_BUILD_BENCHMARKS is on: a plain
# executable timing itself, because what is compared here is a handful of
# nanoseconds per call and no test framework should be standing in that.

wxl_add_benchmark(wxl.fmt.text-builder-benchmark SOURCES text_builder_benchmark.cpp)

# wxl::core for the STA pool the pooled rows allocate from.
target_link_libraries(wxl.fmt.text-builder-benchmark PRIVATE wxl::fmt wxl::core)
