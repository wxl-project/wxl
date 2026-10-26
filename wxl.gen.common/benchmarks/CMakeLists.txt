wxl_add_benchmark(wxl.gen.common.documentation-file-benchmark SOURCES documentation_file_benchmark.cpp)

target_link_libraries(wxl.gen.common.documentation-file-benchmark PRIVATE wxl.gen.common wxl::xml wxl::core)

set_target_properties(wxl.gen.common.documentation-file-benchmark PROPERTIES
    CXX_MODULE_STD ON
    CXX_SCAN_FOR_MODULES ON
)
