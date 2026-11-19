# /cmake/wxl_add_benchmark.cmake

include_guard(GLOBAL)

# A benchmark of the tree, declared the way every one of them is declared:
#
#     wxl_add_benchmark(wxl.xml.document-parse-benchmark
#         SOURCES document_parse_benchmark.cpp
#         ARGS    "${generic_xaml}" 20
#         INPUTS  "${generic_xaml}")
#
# It is a console executable -- a benchmark prints its table -- carrying the
# manifest of wxl's executables (wxl_target_manifest), and it takes its place in
# the IDE tree. What it links and anything else of its own is said after this
# call, on the target it made.
#
# ARGS is the command line it is run with, INPUTS the files it reads; a relative
# input is taken from the directory of the call. Both are written, together with
# the executable of each configuration, into the list tools\bench.ps1 reads:
# <build directory>/benchmarks-<configuration>.json, written when the build files
# are generated, once every benchmark of the configure has been declared. The
# target wxl.benchmarks builds them all.
function(wxl_add_benchmark target)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "SOURCES;ARGS;INPUTS")

    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "wxl_add_benchmark(${target}): unknown arguments ${ARG_UNPARSED_ARGUMENTS}")
    endif()

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "wxl_add_benchmark(${target}): no SOURCES")
    endif()

    add_executable(${target} ${ARG_SOURCES})

    wxl_target_manifest(${target})
    wxl_set_target_folders(${target})

    set(_inputs "")
    foreach(_input IN LISTS ARG_INPUTS)
        cmake_path(ABSOLUTE_PATH _input BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE)
        list(APPEND _inputs "${_input}")
    endforeach()

    set_property(TARGET ${target} PROPERTY WXL_BENCHMARK_ARGS ${ARG_ARGS})
    set_property(TARGET ${target} PROPERTY WXL_BENCHMARK_INPUTS ${_inputs})

    # The first benchmark of a configure makes the aggregate target and asks for
    # the list to be written at the end of the top-level directory, when every
    # other one has been declared too.
    if(NOT TARGET wxl.benchmarks)
        add_custom_target(wxl.benchmarks)
        cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" CALL _wxl_write_benchmark_list)
    endif()

    add_dependencies(wxl.benchmarks ${target})
    set_property(GLOBAL APPEND PROPERTY WXL_BENCHMARKS ${target})
endfunction()

# The strings of a list as the elements of a JSON array.
function(_wxl_json_strings out)
    set(_items "")
    foreach(_item IN LISTS ARGN)
        string(REPLACE "\\" "\\\\" _item "${_item}")
        string(REPLACE "\"" "\\\"" _item "${_item}")
        list(APPEND _items "\"${_item}\"")
    endforeach()
    list(JOIN _items ", " _joined)
    set(${out} "${_joined}" PARENT_SCOPE)
endfunction()

# The list, one file per configuration: the executable is that configuration's.
function(_wxl_write_benchmark_list)
    get_property(_targets GLOBAL PROPERTY WXL_BENCHMARKS)

    set(_entries "")
    foreach(_target IN LISTS _targets)
        get_property(_args TARGET ${_target} PROPERTY WXL_BENCHMARK_ARGS)
        get_property(_inputs TARGET ${_target} PROPERTY WXL_BENCHMARK_INPUTS)
        _wxl_json_strings(_args_json ${_args})
        _wxl_json_strings(_inputs_json ${_inputs})

        list(APPEND _entries
            "  {\n    \"target\": \"${_target}\",\n    \"executable\": \"$<TARGET_FILE:${_target}>\",\n    \"arguments\": [${_args_json}],\n    \"inputs\": [${_inputs_json}]\n  }")
    endforeach()

    list(JOIN _entries ",\n" _body)

    file(GENERATE
        OUTPUT "${CMAKE_BINARY_DIR}/benchmarks-$<CONFIG>.json"
        CONTENT "[\n${_body}\n]\n")
endfunction()
