# /cmake/all.cmake

# ---- Most common infrastructure ----
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(GNUInstallDirs)

# ---- Preset configuration types if not provided via CLI ----
if(WIN32 AND NOT DEFINED CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES Debug Release)
endif()

# The compile options of the language are not here: cmake/wxl_language.cmake
# holds them, and the root sets or checks them right after project().

set(_old_module_path ${CMAKE_MODULE_PATH})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

# --- includes begin ----
# Pure helpers (wxl_add_module_library, wxl_set_target_folders, wxl_add_gtest).
# The same file is installed with the package, so keep it free of global state --
# everything above this line is what makes config.cmake project-local.
include("${CMAKE_CURRENT_LIST_DIR}/wxl_helpers.cmake")
# The documentation site (wxl_add_doxygen_site, wxl_doxygen_tree_inputs): the
# tree's own docs/ and the site of a project that adds wxl. Not installed with
# the package, hence not among the helpers.
include("${CMAKE_CURRENT_LIST_DIR}/wxl_doxygen.cmake")
# The benchmarks of the tree (wxl_add_benchmark): measurement tools of wxl's own
# build, not something a consumer declares, so they do not travel with the package.
include("${CMAKE_CURRENT_LIST_DIR}/wxl_add_benchmark.cmake")
# --- includes end ----

set(CMAKE_MODULE_PATH ${_old_module_path})
unset(_old_module_path)
