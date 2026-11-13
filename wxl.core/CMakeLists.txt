# ---- CMakeList.txt : wxl.core ----
#
# A subdirectory of the tree's project (the root CMakeLists.txt), which sets the
# language and includes the helpers before it.

# No link-time optimization here either, and for the reason spelled out at the
# root of the wxl tree: measured, /GL + /LTCG neither shrinks the binaries nor
# speeds up anything this tree does -- it makes one hot loop slower by inlining
# it across a library boundary -- while the static libraries it produces are an
# order of magnitude larger, since a /GL object holds an intermediate
# representation rather than machine code.

# Precompiled header built from src/pch.h (platform.h + abi.h -- i.e. <windows.h>
# and the fixed-width/assert plumbing). Purely a build-speed knob: every source
# already includes those headers explicitly, so turning it off changes nothing
# but compile time. Kept as an option because it is MSVC-specific behaviour and
# the module-unit interaction is worth being able to bisect out.
option(WXL_ENABLE_PCH "Use a precompiled header (src/pch.h) for wxl.core." ON)

# Off by default: the checks of how coroutines are used -- a task awaited twice at
# once, a value read twice, a task dropped while it runs or while somebody joins it,
# a task used after a move -- are then assert's, a Debug build's alone, and a
# Release build has neither a byte nor an instruction of them. On, they stay in every
# build, Release included, and a broken one ends the process through wxl::core::abort,
# naming the reason and the line of the code that broke the rule where the language
# gives it (std::source_location).
option(WXL_STRICT_CORO "Keep the coroutine checks of the async area in every build (defines STRICT_CORO)." OFF)

add_subdirectory(src)

if(WXL_TESTS)
    add_subdirectory(tests)
endif()

if(WXL_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
elseif(PROJECT_IS_TOP_LEVEL)
    # The bench of the XML reader alone: built whenever the tree is the
    # project, and in a project that adds wxl only with the other benchmarks.
    add_subdirectory(benchmarks/xml)
endif()
