# The document is the Windows SDK's reference for Windows.Foundation.UniversalApiContract,
# in English: the file that documents T:Windows.Foundation.Uri, the member the
# benchmark looks for by default. The SDK is the one the toolchain was set up with --
# WindowsSdkDir and WindowsSDKVersion of the developer environment the configure runs
# in, the SDK the compiler builds against -- not the newest one installed. Its
# References tree is References\<SDK version>\<contract>\<contract version>\<language>,
# with one contract version per SDK.
if(NOT DEFINED ENV{WindowsSdkDir} OR NOT DEFINED ENV{WindowsSDKVersion})
    message(FATAL_ERROR
        "wxl.gen.common.documentation-file-benchmark: no WindowsSdkDir or WindowsSDKVersion in "
        "the environment -- configure from a developer environment (tools\\build.ps1 sets one up).")
endif()

file(TO_CMAKE_PATH "$ENV{WindowsSdkDir}" _wxl_doc_benchmark_kits)
string(REGEX REPLACE "[\\/]+$" "" _wxl_doc_benchmark_sdk "$ENV{WindowsSDKVersion}")
set(_wxl_doc_benchmark_contract
    "${_wxl_doc_benchmark_kits}/References/${_wxl_doc_benchmark_sdk}/Windows.Foundation.UniversalApiContract")

file(GLOB _wxl_doc_benchmark_versions LIST_DIRECTORIES true "${_wxl_doc_benchmark_contract}/*")
list(FILTER _wxl_doc_benchmark_versions INCLUDE REGEX "/[0-9]+(\\.[0-9]+)*$")
list(LENGTH _wxl_doc_benchmark_versions _wxl_doc_benchmark_count)
if(NOT _wxl_doc_benchmark_count EQUAL 1)
    message(FATAL_ERROR
        "wxl.gen.common.documentation-file-benchmark: expected one contract version in "
        "${_wxl_doc_benchmark_contract}, found ${_wxl_doc_benchmark_count}")
endif()

set(_wxl_doc_benchmark_document
    "${_wxl_doc_benchmark_versions}/en/Windows.Foundation.UniversalApiContract.xml")
if(NOT EXISTS "${_wxl_doc_benchmark_document}")
    message(FATAL_ERROR
        "wxl.gen.common.documentation-file-benchmark: no ${_wxl_doc_benchmark_document}")
endif()

wxl_add_benchmark(wxl.gen.common.documentation-file-benchmark
    SOURCES documentation_file_benchmark.cpp
    ARGS    "${_wxl_doc_benchmark_document}"
    INPUTS  "${_wxl_doc_benchmark_document}"
)

target_link_libraries(wxl.gen.common.documentation-file-benchmark PRIVATE wxl.gen.common wxl::xml wxl::core)

set_target_properties(wxl.gen.common.documentation-file-benchmark PROPERTIES
    CXX_MODULE_STD ON
    CXX_SCAN_FOR_MODULES ON
)
