# Перенос WinUI Gallery на wxl: те же типы, без XAML. Страница контрола — по
# .cpp; общая раскладка примера (ControlExample оригинала) — в Presenter.cpp;
# окно с NavigationView — в main.cpp.

# Сниппеты: код примеров (X.h) страницы компилируют #include, а для показа
# эти же X.h и введения (X.html) читает рядом с exe LazyLoadedText.
file(GLOB_RECURSE _gallery_snippets CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/Snippets/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/Snippets/*.html")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Snippets" PREFIX "Snippets"
             FILES ${_gallery_snippets})

wxl_add_executable(sample.gallery
    UI wxl::ui.full
    Pages.h
    Catalog.h
    Shell.h
    pch.h
    main.cpp
    Presenter.cpp
    Prose.cpp
    LazyText.cpp
    Catalog.cpp
    Contact.cpp
    CustomDataObject.cpp
    Layouts.cpp
    MotionPages.cpp
    LottieLogo.cpp
    NavigationPages.cpp
    TabViewWindowing.cpp
    PagedFrame.cpp
    RepeaterData.cpp
    Shell.cpp
    Tiles.cpp
    PageRegistry.cpp
    ItemPage.cpp
    ListPages.cpp
    HomePage.cpp
    SettingsPage.cpp
    ButtonPage.cpp
    DropDownButtonPage.cpp
    HyperlinkButtonPage.cpp
    RepeatButtonPage.cpp
    ToggleButtonPage.cpp
    SplitButtonPage.cpp
    ToggleSplitButtonPage.cpp
    CheckBoxPage.cpp
    ColorPickerPage.cpp
    ComboBoxPage.cpp
    RadioButtonPage.cpp
    RatingControlPage.cpp
    SliderPage.cpp
    ToggleSwitchPage.cpp
    TextBlockPage.cpp
    TextBoxPage.cpp
    PasswordBoxPage.cpp
    NumberBoxPage.cpp
    AutoSuggestBoxPage.cpp
    BorderPage.cpp
    CanvasPage.cpp
    ExpanderPage.cpp
    GridPage.cpp
    RelativePanelPage.cpp
    SplitViewPage.cpp
    StackPanelPage.cpp
    VariableSizedWrapGridPage.cpp
    ViewboxPage.cpp
    DesignPages.cpp
    Design.cpp
    ColorPage.cpp
    IconographyPage.cpp
    AccessibilityColorContrastPage.cpp
    AccessibilityKeyboardPage.cpp
    AccessibilityScreenReaderPage.cpp
    ScratchPadPage.cpp
    XamlStylesPage.cpp
    GlassPage.cpp
    MagnifyPage.cpp
    BevelPage.cpp
    Button3DPage.cpp
    GaussianBlurPage.cpp
    HaloPage.cpp
    XamlResourcesPage.cpp
    BindingPage.cpp
    BoundCollectionPage.cpp
    SettingsCardPage.cpp
    HeaderedContentControlPage.cpp
    SettingsExpanderPage.cpp
    TemplatesPage.cpp
    CustomXamlConditionalsPage.cpp
    Icons.cpp
    ColorTile.cpp
    ColorSamples.cpp
    ColorSections.cpp
    AnimatedVisualPlayerPage.cpp
    CaptureElementPreviewPage.cpp
    InkCanvasPage.cpp
    MapControlPage.cpp
    MediaPlayerElementPage.cpp
    SoundPage.cpp
    PersonPicturePage.cpp
    ImagePage.cpp
    TabViewPage.cpp
    NavigationViewPage.cpp
    PivotPage.cpp
    SelectorBarPage.cpp
    BreadcrumbBarPage.cpp
    ConnectedAnimationPage.cpp
    XamlCompInteropPage.cpp
    ParallaxViewPage.cpp
    PageTransitionPage.cpp
    ThemeTransitionPage.cpp
    ImplicitTransitionPage.cpp
    EasingFunctionPage.cpp
    TeachingTipPage.cpp
    PopupPage.cpp
    FlyoutPage.cpp
    ContentDialogPage.cpp
    ItemsRepeaterPage.cpp
    ItemsViewPage.cpp
    ListViewPage.cpp
    GridViewPage.cpp
    FlipViewPage.cpp
    TreeViewPage.cpp
    PullToRefreshPage.cpp
    ContentIslandPage.cpp
    JumpListPage.cpp
    StoragePickersPage.cpp
    ClipboardPage.cpp
    AppNotificationPage.cpp
    BadgeNotificationManagerPage.cpp
    CompositionWindowPage.cpp
    SystemBackdropsPage.cpp
    AppWindowPage.cpp
    AppWindowTitleBarPage.cpp
    TitleBarPage.cpp
    SamplePages.cpp
    ColorSelector.cpp
    WindowingPage.cpp
    CompactSizingPage.cpp
    SystemBackdropElementPage.cpp
    AcrylicPage.cpp
    AnimatedIconPage.cpp
    RadialGradientBrushPage.cpp
    LinePage.cpp
    ShapePage.cpp
    IconElementPage.cpp
    ThemeShadowPage.cpp
    ScrollViewPage.cpp
    AnnotatedScrollBarPage.cpp
    PipsPagerPage.cpp
    PagerControlPage.cpp
    ScrollViewerPage.cpp
    CalendarViewPage.cpp
    TimePickerPage.cpp
    DatePickerPage.cpp
    CalendarDatePickerPage.cpp
    StandardUICommandPage.cpp
    XamlUICommandPage.cpp
    SwipeControlPage.cpp
    CommandBarFlyoutPage.cpp
    MenuFlyoutPage.cpp
    MenuBarPage.cpp
    CommandBarPage.cpp
    LayoutPanelPage.cpp
    WrapPanelPage.cpp
    AppBarSeparatorPage.cpp
    AppBarToggleButtonPage.cpp
    AppBarButtonPage.cpp
    ToolTipPage.cpp
    ProgressRingPage.cpp
    ProgressBarPage.cpp
    InfoBarPage.cpp
    InfoBadgePage.cpp
    RichTextBlockPage.cpp
    RichEditBoxPage.cpp
    ${_gallery_snippets}
)

wxl_target_assets(sample.gallery Assets)
# Сниппеты рядом с exe: их читает LazyLoadedText при первом обращении.
wxl_target_assets(sample.gallery Snippets)
# Версия приложения — версия проекта из корневого CMakeLists.txt: у приложения без пакета других источников нет.
target_compile_definitions(sample.gallery PRIVATE GALLERY_VERSION=u"${wxl-project_VERSION}")
target_link_libraries(sample.gallery PRIVATE wxl::core)

# LottieLogo.cpp -- код анимации Lottie, написанный на объектах Composition и Win2D самой платформы, -- единственный файл
# приложения, которому нужны заголовки проекции; наружу он отдаёт один wxl::Object. Остальным они остаются закрыты:
# каталог проекции (каталог включаемых файлов цели Microsoft::CppWinRT) задан только этому файлу, а саму цель
# приложение не линкует.
set_source_files_properties(LottieLogo.cpp PROPERTIES INCLUDE_DIRECTORIES
    "$<TARGET_PROPERTY:Microsoft::CppWinRT,INTERFACE_INCLUDE_DIRECTORIES>;${WXL_WINDOWSAPPSDK_FOUNDATION_DIR}/include;${WXL_WINDOWSAPPSDK_WINUI_DIR}/include")
