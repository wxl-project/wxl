# /cmake/wxl_language.cmake
#
# The language wxl is compiled in, which every translation unit that imports a
# wxl module has to share with it:
#
#   WXL_LANGUAGE_OPTIONS          the compile options (Windows), for a whole
#                                 directory rather than a target
#   WXL_WARNING_OPTIONS           the warning level of wxl's own targets
#   wxl_apply_language_options()  sets them, the warning level and the language
#                                 level in the calling directory: the root of
#                                 the wxl tree does, when the tree is the
#                                 top-level project
#   wxl_check_language_options()  checks that the calling directory has them,
#                                 and stops with the lines to add when it has
#                                 not: the root does, when a project adds it
#
# The language level is C++23, required, without extensions, with the sources
# scanned for imports (CMAKE_CXX_SCAN_FOR_MODULES), which is what gives a
# target the `std` module.
#
# Why one set for wxl and its consumers: a target that imports `std` gets the
# module built for what affects its use (Source/cmCxxModuleUsageEffects.cxx of
# CMake, read at v4.5.0): a hash, per configuration, of the target's compile
# options -- its own, its directory's and those it takes from what it links,
# but not warning options (/w, /W, /external:) nor preprocessor ones (/D, /I,
# /U) -- of its CXX_STANDARD, CXX_EXTENSIONS and CXX_STANDARD_REQUIRED, and of
# its MSVC runtime library. Each distinct hash gets a copy of the module of its
# own. Compile definitions and CMAKE_CXX_FLAGS are not hashed.
#
# So every target that imports a wxl module shares one copy only when all of
# them carry the same options and level, which directory options set before
# the first target give them. Not a usage requirement of wxl::core: wxl::core
# carries /Gw in Release only, so its consumers hash like any other target in
# Debug and apart from them in Release. With these options on the target too,
# its consumers would differ from the rest in both configurations, and the
# `std` module would be built once more in each.
#
# The check verifies the mechanism this file prescribes, the COMPILE_OPTIONS of
# the directory, and nothing else: options a project carries through
# CMAKE_CXX_FLAGS (or CMAKE_CXX_FLAGS_INIT) reach the command line but not the
# hash, so such a project is still asked to add them as directory options. The
# command line then carries them twice, and the directory options are the
# spelling the hash sees.
#
# /Zc:throwingNew tells the compiler that allocation functions follow the
# standard: one that is not noexcept never returns null. The pool never does --
# it stops the process instead. As of MSVC 14.53.36911 the compiler drops its
# null test only after the global operator new; after a class-specific one, the
# pool's included, the test still stands (checked in the listing). The option
# is kept to state the intent, and a compiler that honours it for
# class-specific allocation functions gets it for free.
#
# Why a project that adds wxl sets them itself: the compile options of a
# directory reach the targets created in it after they are set and in the
# directories it adds after that, and nothing above. Set by wxl, they would
# reach wxl's targets and not the project's; set by wxl on the project's
# directory, they would change the project's other targets behind its back. So
# the check names what is missing and stops, and the project adds the lines
# above the add_subdirectory (or FetchContent_MakeAvailable) of wxl.
#
# No include guard: the file sets the list in the scope that includes it, and
# the functions read it from there.

set(WXL_LANGUAGE_OPTIONS /permissive- /utf-8 /Zc:throwingNew)

# The language level, CMAKE_CXX_STANDARD: the version of the standard, without
# extensions.
set(WXL_CXX_STANDARD 23)

# The warning level wxl's own targets, its samples and its sandbox are built at.
# Not part of the language: warning options are not hashed by CMake, so a
# project that adds wxl keeps the level of its own, and the check above does not
# ask for it.
set(WXL_WARNING_OPTIONS /W4)

function(wxl_apply_language_options)
    set(CMAKE_CXX_STANDARD ${WXL_CXX_STANDARD} PARENT_SCOPE)
    set(CMAKE_CXX_STANDARD_REQUIRED ON PARENT_SCOPE)
    set(CMAKE_CXX_EXTENSIONS OFF PARENT_SCOPE)
    set(CMAKE_CXX_SCAN_FOR_MODULES ON PARENT_SCOPE)

    if(WIN32)
        add_compile_options(${WXL_LANGUAGE_OPTIONS} ${WXL_WARNING_OPTIONS})
    endif()
endfunction()

function(wxl_check_language_options)
    # What is missing, as the project has it, and the lines that add it.
    set(missing "")
    set(lines "")

    if(WIN32)
        get_directory_property(options COMPILE_OPTIONS)
        set(absent "")
        foreach(option IN LISTS WXL_LANGUAGE_OPTIONS)
            if(NOT option IN_LIST options)
                list(APPEND absent "${option}")
            endif()
        endforeach()
        if(absent)
            list(JOIN absent " " absent)
            string(APPEND missing "\n  compile options of the directory: no ${absent}")
            string(APPEND lines "\n    add_compile_options(${absent})")
        endif()
    endif()

    if(NOT CMAKE_CXX_STANDARD STREQUAL WXL_CXX_STANDARD)
        string(APPEND missing
            "\n  CMAKE_CXX_STANDARD: \"${CMAKE_CXX_STANDARD}\", which is not ${WXL_CXX_STANDARD}")
        string(APPEND lines "\n    set(CMAKE_CXX_STANDARD ${WXL_CXX_STANDARD})")
    endif()
    if(NOT CMAKE_CXX_STANDARD_REQUIRED)
        string(APPEND missing
            "\n  CMAKE_CXX_STANDARD_REQUIRED: \"${CMAKE_CXX_STANDARD_REQUIRED}\", which is not true")
        string(APPEND lines "\n    set(CMAKE_CXX_STANDARD_REQUIRED ON)")
    endif()
    if(NOT DEFINED CMAKE_CXX_EXTENSIONS OR CMAKE_CXX_EXTENSIONS)
        string(APPEND missing
            "\n  CMAKE_CXX_EXTENSIONS: \"${CMAKE_CXX_EXTENSIONS}\", which is not set false")
        string(APPEND lines "\n    set(CMAKE_CXX_EXTENSIONS OFF)")
    endif()
    if(NOT CMAKE_CXX_SCAN_FOR_MODULES)
        string(APPEND missing
            "\n  CMAKE_CXX_SCAN_FOR_MODULES: \"${CMAKE_CXX_SCAN_FOR_MODULES}\", which is not true")
        string(APPEND lines "\n    set(CMAKE_CXX_SCAN_FOR_MODULES ON)")
    endif()

    if(lines)
        message(FATAL_ERROR
            "\n[ERROR]: ${CMAKE_SOURCE_DIR} adds wxl, and the directory that adds it does not set "
            "the language wxl is compiled in:"
            "${missing}\n"
            "\n"
            "Every translation unit that imports a wxl module shares wxl's language options, and "
            "wxl does not set them in a project that adds it: that would change the project's "
            "other targets. Add these lines ABOVE the add_subdirectory (or "
            "FetchContent_MakeAvailable) of wxl:"
            "${lines}\n"
            "\n"
            "cmake/wxl_language.cmake in the wxl tree says why.\n")
    endif()
endfunction()
