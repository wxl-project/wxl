# The reference document is WinUI3's generic.xaml, from the WinUI package the
# tree builds against. Found by the same call wxl.ui makes for
# WXL_WINDOWSAPPSDK_WINUI_DIR: wxl.xml is configured before wxl.ui, and on its
# own without it, so that variable is not there yet.
include(${WXL_CMAKE_SCRIPT_DIR}/wxl_packages.cmake)
wxl_nuget_package_dir(_wxl_xml_bench_winui_dir Microsoft.WindowsAppSDK.WinUI)
set(_wxl_xml_bench_document "${_wxl_xml_bench_winui_dir}/lib/native/Microsoft.UI/Themes/generic.xaml")

wxl_add_benchmark(wxl.xml.bench
    SOURCES bench.cpp
    ARGS    "${_wxl_xml_bench_document}"
    INPUTS  "${_wxl_xml_bench_document}"
)

target_link_libraries(wxl.xml.bench PRIVATE wxl::xml wxl::core)

# Consuming a module built with `import std;` means being scanned for imports
# and getting the std module too; scanning an ordinary source is CMP0155-gated
# and the project leaves that policy at OLD.
set_target_properties(wxl.xml.bench PROPERTIES
    CXX_MODULE_STD ON
    CXX_SCAN_FOR_MODULES ON
)
