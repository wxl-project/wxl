# The samples open wxl::dsl at file scope, as an application does, so a local
# named like a tag is expected there and C4459 says nothing about it.
add_compile_options(/wd4459)

add_subdirectory(Calculator)
add_subdirectory(CustomTitleBar)
add_subdirectory(Gallery)
add_subdirectory(HelloHere)
add_subdirectory(HtmlView)
add_subdirectory(Quadratic)
add_subdirectory(Trayed)
