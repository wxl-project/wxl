# Проба темы окна: что стоит в дереве над Window.Content, живёт ли этот предок дольше
# содержимого, наследует ли новое содержимое тему, заданную на нём, и откуда тему берут
# Flyout и ContentDialog. Отвечает в theme-probe.txt рядом с исполняемым файлом и
# закрывается сама. Видит проекцию cppwinrt напрямую, поэтому живёт здесь, а не в samples.
wxl_add_executable(sandbox.theme-probe
    UI wxl::ui.full
    main.cpp
)

target_link_libraries(sandbox.theme-probe PRIVATE Microsoft::CppWinRT runtimeobject.lib)
