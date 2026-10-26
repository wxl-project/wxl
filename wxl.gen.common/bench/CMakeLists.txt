wxl_add_benchmark(wxl.gen.common.bench SOURCES bench.cpp)

target_link_libraries(wxl.gen.common.bench PRIVATE wxl.gen.common wxl::xml wxl::core)

set_target_properties(wxl.gen.common.bench PROPERTIES
    CXX_MODULE_STD ON
    CXX_SCAN_FOR_MODULES ON
)
