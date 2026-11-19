# The reference document is WinUI's generic.xaml, from the WinUI package the
# tree builds against: the package the restore of the root resolved
# (cmake/wxl_packages.cmake), found by the same call wxl.ui makes for
# WXL_WINDOWSAPPSDK_WINUI_DIR.
wxl_nuget_package_dir(_wxl_xml_benchmark_winui_dir Microsoft.WindowsAppSDK.WinUI)
set(_wxl_xml_benchmark_document "${_wxl_xml_benchmark_winui_dir}/lib/native/Microsoft.UI/Themes/generic.xaml")

wxl_add_benchmark(wxl.xml.document-parse-benchmark
    SOURCES document_parse_benchmark.cpp
    ARGS    "${_wxl_xml_benchmark_document}"
    INPUTS  "${_wxl_xml_benchmark_document}"
)

target_link_libraries(wxl.xml.document-parse-benchmark PRIVATE wxl::core)

# Consuming a module built with `import std;` means being scanned for imports,
# which gives the std module too; scanning an ordinary source is CMP0155-gated
# and the project leaves that policy at OLD.
set_target_properties(wxl.xml.document-parse-benchmark PROPERTIES
    CXX_SCAN_FOR_MODULES ON
)
