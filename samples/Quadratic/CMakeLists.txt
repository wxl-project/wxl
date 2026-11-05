# Квадратное уравнение: модель на observable и интерфейс одним выражением в окне
# CompositionWindow с фоном плиткой, всё в main.cpp. Как любое приложение на wxl,
# линкует wxl::ui.full и, ради format(), wxl::core; WIN32 делает точкой входа wWinMain
# самой wxl.

wxl_add_executable(sample.quadratic
    UI wxl::ui.full
    main.cpp
)

target_link_libraries(sample.quadratic PRIVATE wxl::core)

wxl_target_icon(sample.quadratic app.ico)
wxl_target_assets(sample.quadratic Assets)
