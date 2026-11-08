// Compile-time check of the declarative surface of the composition window,
// the drawing surfaces, the texture cache and the theme brush.
// Built into the surface test of every profile that selects the component,
// beside test/dsl_surface.cpp; nothing here runs.

#include "CompositionWindow.h"
#include "DrawingSurface.h"
#include "TextureCache.h"
#include "ThemeBrush.h"
#include "ui.h"
#include "event_awaitable.h"
#include "method.h"
#include <wxl/Microsoft.UI.Xaml.Hosting.h>

namespace {

using namespace wxl;
using namespace wxl::dsl;

// CompositionWindow takes the window's material under the island, as Window does.
[[maybe_unused]] void composition_window_backdrop() {
    CompositionWindow const window{systemBackdrop = MicaBackdrop{}};
    window.systemBackdrop(MicaBackdrop{});
}

// wxl's own window written the same way: a handle, so every member is const
// and the tags apply to it; its title bar is placed by the window itself, so
// it is built right in the braces; its own events take the same tags, and a
// copy captured by a lambda is the same window.
[[maybe_unused]] void composition_window(TitleBar const& built, Grid const& page,
                                         async::cancellation_token token) {
    CompositionWindow window{
        title = L"App",
        minSize = {820, 560},
        extendsContentIntoTitleBar = true,
        titleBar = {leftHeader = TextBlock{L"App"}, rightHeader = Button{L"Sign in"}},
        zoomFactor = 1.25,
        onClosing +=
        [](Object const&, WindowClosingEventArgs& args) { args.cancel(!args.cancel()); },
        onClosed += [] {},
        onGeometryChanged += [] {},
        onClientSizeChanged += [](Object const&, ClientSize const& client) { (void)client.scale; },
        onKeyDown += [](Object const&, VirtualKey const& key) { (void)key; },
        page,
    };
    CompositionWindow routed{built, page};
    CompositionWindow const legacy{L"App", SizeInt32{820, 560}};

    // The scene's background is a tag as well: a colour, or a picture with the
    // way it covers the window and the colour under it.
    CompositionWindow const tiled{
        background = BackgroundImage{L"Assets/paper.png", BackgroundFill::TileMirrored},
        page,
    };
    CompositionWindow const coloured{background = rgb(32, 32, 32)};
    // A picture loaded on the way, under the token the exit ends its wait with.
    tiled.backgroundAsync(BackgroundImage{L"Assets/photo.png", BackgroundFill::None, rgb(0, 0, 0)},
                          token);
    coloured.backgroundAsync(std::filesystem::path{L"Assets/photo.png"}, token);

    auto const closeLater = [window] { window.close(); };
    (void)closeLater;
    window.zoomFactor(window.zoomFactor() * 1.1);
    window.appWindow().title(legacy.title());
    auto sizes = wxl::on_event<EventKey::ClientSizeChanged>(window);
    (void)sizes;

    // A request to close, refusable, awaited the same way: its args arrive by
    // reference, to be answered before the coroutine next suspends.
    auto closings = wxl::on_event<EventKey::Closing>(window);
    static_assert(std::is_same_v<decltype(closings)::args_ref_t, WindowClosingEventArgs&>);
    (void)closings;
}

// backgroundAsync takes the token its load is registered with, and has no form without one: the
// wait for the picture is a frame that has to be over before the exit.
template <class Image>
concept loads_background_without_a_token = requires(
    CompositionWindow const& window, Image const& image) { window.backgroundAsync(image); };

static_assert(!loads_background_without_a_token<BackgroundImage> &&
              !loads_background_without_a_token<std::filesystem::path>);

// A texture from the cache, under a token as every operation of the platform's.
[[maybe_unused]] async::task<CompositionDrawingSurface> cached_texture(
    TextureCache& cache, async::cancellation_token token) {
    return cache.getAsync(std::filesystem::path{L"C:/Pictures/photo.png"}, std::move(token));
}

// An application that refuses to close until its own exit is over: the handler is
// a member, and it answers through the args.
struct RefusesUntilDone {
    bool finished = false;

    void closing(Object const&, WindowClosingEventArgs& args) const {
        if (!finished) args.cancel(true);
    }

    void build() const {
        CompositionWindow window{onClosing += method(this, &RefusesUntilDone::closing)};
        window.add_onClosing(method(this, &RefusesUntilDone::closing));
    }
};

// An owner that keeps its watches on the window's events as members, built with the owner: each
// calls a noexcept member of its own, with the event's args or with nothing, and comes off with
// the owner. A remove of the window answers whether a token was there.
struct WatchesTheWindow {
    explicit WatchesTheWindow(CompositionWindow const& window)
        : geometryWatch{window, *this},
          sizeWatch{window, *this},
          closingWatch{window, *this},
          keyWatch{window, *this} {}

    void moved() noexcept {}
    void sized(ClientSize const& client) noexcept { (void)client.scale; }
    void closing(WindowClosingEventArgs& args) noexcept { args.cancel(true); }
    void keyPressed(VirtualKey const& key) noexcept { (void)key; }

    void placed(CompositionWindow const& window) {
        EventToken token = window.add_onGeometryChanged([] {});
        bool const removed = window.remove_onGeometryChanged(token);
        (void)removed;
    }

    WindowWatch<EventKey::GeometryChanged, &WatchesTheWindow::moved> geometryWatch;
    WindowWatch<EventKey::ClientSizeChanged, &WatchesTheWindow::sized> sizeWatch;
    WindowWatch<EventKey::Closing, &WatchesTheWindow::closing> closingWatch;
    WindowWatch<EventKey::KeyDown, &WatchesTheWindow::keyPressed> keyWatch;
};

[[maybe_unused]] void window_watches(CompositionWindow const& window) {
    WatchesTheWindow watching{window};
    watching.placed(window);
}

// The application's own Direct2D on a surface the compositor shows.
//
// Everything below the last line is interop -- a D3D11 device, a D2D device,
// ICompositorInterop, BeginDraw -- and none of it appears here, which is the
// point: what reaches an application is a size in pixels, a callback holding
// a device context, and a brush.
//
// ID2D1DeviceContext is only forward-declared by DrawingSurface.h, so this
// translation unit names the pointer without parsing Direct2D at all.
void own_drawing(Compositor const& compositor, FrameworkElement const& host, XamlRoot const& root) {
    auto const scale = static_cast<float>(root.rasterizationScale());
    auto const width = static_cast<float>(host.actualWidth());
    auto const height = static_cast<float>(host.actualHeight());
    SizeInt32 const wanted{static_cast<int32_t>(width * scale),
                           static_cast<int32_t>(height * scale)};

    DrawingSurface page{compositor, wanted};
    page.resize(wanted);
    SizeInt32 const pixels = page.size();
    (void)pixels;

    // False: nothing drawn -- no size yet, or the device is lost and the watch below
    // will say when to draw again. A Direct2D call inside the paint hands its answer to
    // throwIfDeviceLost before deciding what a failure means; DeviceLost ends the paint,
    // and draw() catches it.
    bool const drew = page.draw([](ID2D1DeviceContext*) { DrawingSurface::throwIfDeviceLost(0); });
    (void)drew;
    DrawingSurface::DeviceLost const lost;
    std::exception const& named = lost;
    (void)named.what();

    SpriteVisual sprite = compositor.createSpriteVisual();
    sprite.brush(page.brush());
    sprite.size({width, height});

    // The brush has a transform of its own, over the picture it paints: one
    // drawn surface turned, moved or scaled inside the sprite, in the
    // sprite's own units. Matrix3x2 is a wxl type like the vectors, and a
    // braced literal is the whole of writing one.
    CompositionSurfaceBrush turned = page.brush();
    turned.centerPoint({width / 2, height / 2});
    turned.rotationAngleInDegrees(180.0f);
    turned.rotationAngle(3.14159265f);
    turned.scale({1, 1});
    turned.offset({0, 0});
    turned.anchorPoint({0, 0});
    turned.transformMatrix({-1, 0, 0, -1, width, height});
    Matrix3x2 const placed = turned.transformMatrix();
    (void)placed;
    ElementCompositionPreview::setElementChildVisual(host, sprite);
}

// An owner of surfaces hears that the device under them was replaced: a member
// of its own, noexcept and taking nothing, held by a watch on the compositor the
// surfaces were made on -- where it drops what it made from the old device and
// draws again.
struct DrawnPage {
    explicit DrawnPage(Compositor const& compositor) : deviceWatch_{compositor, *this} {}

    void deviceReplaced() noexcept {}

    RenderingDeviceWatch<&DrawnPage::deviceReplaced> deviceWatch_;
};

void device_replaced(Compositor const& compositor) {
    DrawnPage const page{compositor};
    static_assert(
        !std::is_default_constructible_v<RenderingDeviceWatch<&DrawnPage::deviceReplaced>>);
    (void)page;
}

// A surface longer than any texture of the device, drawn in parts: the boundaries of the
// whole, a part no longer than the device draws at once, painted in the coordinates of the
// whole, and the parts to keep named when the rest can go. The device is the same as a
// DrawingSurface's, and so is its loss.
void own_drawing_in_parts(Compositor const& compositor, FrameworkElement const& host) {
    VirtualDrawingSurface text{compositor, SizeInt32{640, 60000}};
    text.resize(SizeInt32{640, 80000});
    SizeInt32 const pixels = text.size();
    std::int32_t const most = text.maxUpdateSide();

    RectInt32 const part{0, 40000, pixels.width, most < 1024 ? most : 1024};
    bool const drew =
        text.draw(part, [](ID2D1DeviceContext*) { DrawingSurface::throwIfDeviceLost(0); });
    (void)drew;
    RectInt32 const kept[] = {part};
    text.trim(kept);
    text.trim({});

    SpriteVisual sprite = compositor.createSpriteVisual();
    sprite.brush(text.brush());
    sprite.size({640.0f, 80000.0f});
    ElementCompositionPreview::setElementChildVisual(host, sprite);
}

// The same surface as the member pointers say it: drawing and trimming change pixels, not
// the wrapper, and the longest side is a question of the device the surface is on.
using SurfacePaint = std::function<void(ID2D1DeviceContext*)>;
static_assert(VirtualDrawingSurface::maxSide == 16777216);
static_assert(
    std::is_same_v<decltype(&VirtualDrawingSurface::draw),
                   bool (VirtualDrawingSurface::*)(RectInt32, SurfacePaint const&) const>);
static_assert(std::is_same_v<decltype(&VirtualDrawingSurface::trim),
                             void (VirtualDrawingSurface::*)(std::span<RectInt32 const>) const>);
static_assert(std::is_same_v<decltype(&VirtualDrawingSurface::maxUpdateSide),
                             std::int32_t (VirtualDrawingSurface::*)() const>);
static_assert(std::is_same_v<decltype(&VirtualDrawingSurface::resize),
                             void (VirtualDrawingSurface::*)(SizeInt32)>);

// A brush of the framework's own, replaced for one control. What is proved
// here is the route: it is an unnamed argument like any other, so it goes into
// a description, into a preset, and into an Apply without any of the three
// knowing about it -- and it is refused on an object that has no resources of
// its own, which is everything that is not a FrameworkElement.
[[maybe_unused]] void theme_brushes_go_in_the_braces() {
    Button{
        content = L"Go",
        background = SolidColorBrush{rgb(16, 18, 21)},
        ThemeBrush{L"ButtonBackgroundPointerOver", SolidColorBrush{rgb(27, 31, 37)}},
    };

    auto const look = Preset{
        ThemeBrush{L"ButtonBackgroundPressed", SolidColorBrush{rgb(9, 10, 12)}},
    };

    Button const button{look};

    Apply{button, ThemeBrush{L"ButtonForegroundPointerOver", SolidColorBrush{rgb(255, 255, 255)}}};

    static_assert(!std::invocable<ThemeBrush, SolidColorBrush const&>);
}

// A theme brush takes part in a description, so the comma is deleted for it
// too (the_comma_is_not_a_separator in dsl_surface.cpp).
static_assert(impl::describes<ThemeBrush>);

}  // namespace
