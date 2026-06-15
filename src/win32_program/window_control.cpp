#include "win32_program/window_control.hpp"
#include "win32_program/control_setup.hpp"
#include <windowsx.h>
#include <algorithm>
#include <commctrl.h>
#include <thread>

win32_program::SectionState& win32_program::GetSectionState()
{
    static SectionState SectionState;
    return SectionState;
}

void win32_program::InitializeMenuControls(win32_program::Win32Context& context)
{
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_BAR_CLASSES };    
    InitCommonControlsEx(&icc);

    win32_program::SetupMenuBar(context);
    win32_program::SetupToolbar(context);
}

void win32_program::CreateMainWindowContents(HWND hwnd, Win32Context &context)
{
    WNDCLASSEX wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = context.hInstance;
    wc.lpszClassName = L"SectionWindow";
    RegisterClassEx(&wc);

    auto makeSection = [&](LPCWSTR name, win32_program::ControlId id)
    {
        return CreateWindowEx(
            0, L"SectionWindow", name,
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS,
            0,0,0,0,
            hwnd, (HMENU)id, context.hInstance, nullptr
        );
    };

    auto makeSplitter = [&](win32_program::ControlId id)
    {
        return CreateWindowEx(
            0, L"STATIC", nullptr,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
            0,0,0,0,
            hwnd, (HMENU)id, context.hInstance, nullptr
        );
    };

    context.hLayerListView = makeSection(L"Layer List", win32_program::ControlId::LayerList);
    context.hPackageView   = makeSection(L"Package View", win32_program::ControlId::PackageView);
    context.hMapView       = makeSection(L"Map View", win32_program::ControlId::MapView);
    context.hTilesetView   = makeSection(L"Tileset", win32_program::ControlId::TilesetView);

    context.hSplitLeft = makeSplitter(win32_program::ControlId::SplitLeft);
    context.hSplitRight = makeSplitter(win32_program::ControlId::SplitRight);
    context.hSplitBottom = makeSplitter(win32_program::ControlId::SplitBottom);

}

bool win32_program::CheckDragging(HWND hwnd, LPARAM lParam, Win32Context& context)
{
    HWND child = ChildWindowFromPoint(hwnd, { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
    auto& state = GetSectionState();

    if (child == context.hSplitLeft)
    {
        state.draggingLayerHorizontal = true;        
    }
    else if (child == context.hSplitRight)
    {
        state.draggingTileset = true;        
    }
    else if (child == context.hSplitBottom)
    {
        state.draggingLayerVertical = true;        
    }

    return state.draggingLayerHorizontal || state.draggingTileset || state.draggingLayerVertical;
}

void win32_program::HandleDragging(HWND hwnd, LPARAM lParam, Win32Context &context)
{
    auto& state = GetSectionState();    

    if (state.draggingLayerHorizontal)
    {
        int x = GET_X_LPARAM(lParam);
        state.layerWidth = std::max(x, defaults::minCollumnWidth);
        state.layerWidth = std::min(state.layerWidth,
                                    state.windowWidth - state.tilesetWidth - 2*defaults::minCollumnWidth);
        state.layerHorizontalRatio = (float)state.layerWidth / state.windowWidth;

        LayoutContent(hwnd, context);
    }
    else if (state.draggingTileset)
    {
        int x = GET_X_LPARAM(lParam);
        state.tilesetWidth = std::max(state.windowWidth - x, defaults::minCollumnWidth);
        state.tilesetWidth = std::min(state.tilesetWidth,
                                      state.windowWidth - state.layerWidth - 2*defaults::minCollumnWidth);
        state.tilesetRatio = (float)state.tilesetWidth / state.windowWidth;

        LayoutContent(hwnd, context);
    }
    else if (state.draggingLayerVertical)
    {
        int y = GET_Y_LPARAM(lParam);
        int yLocal = y - state.toolbarOffset;
        state.layerHeight = std::max(yLocal, defaults::minCollumnHeight);
        state.layerHeight = std::min(state.layerHeight,
                                     state.windowHeight - state.toolbarOffset - 2*defaults::minCollumnHeight);
        state.layerVerticalRatio = (float)state.layerHeight / state.windowHeight;

        LayoutContent(hwnd, context);
    }
}

void win32_program::HandleResize(HWND hwnd, LPARAM lParam, Win32Context& context)
{
    auto defer = [](HDWP dwp, HWND handle, int x, int y, int w, int h){
        return DeferWindowPos(
            dwp, handle, nullptr,
            x, y, w, h,
            SWP_NOZORDER
        );
    };

    RECT rc;
    GetClientRect(hwnd, &rc);

    HDWP hdwp = BeginDeferWindowPos(2);
    hdwp = DeferWindowPos(hdwp, context.hRebarTop,    nullptr,
                        0, 0, rc.right, 0,
                        SWP_NOZORDER);
    hdwp = DeferWindowPos(hdwp, context.hRebarBottom, nullptr,
                        0, 0, rc.right, 0,
                        SWP_NOZORDER);
    EndDeferWindowPos(hdwp);

    auto& state = GetSectionState();

    state.windowWidth  = rc.right;
    state.windowHeight = rc.bottom;    

    // Recompute widths/heights from ratios
    state.layerWidth   = (int)(state.layerHorizontalRatio * state.windowWidth);
    state.tilesetWidth = (int)(state.tilesetRatio * state.windowWidth);
    state.layerHeight  = (int)(state.layerVerticalRatio * state.windowHeight);

    // Clamp
    state.layerWidth = std::max(state.layerWidth, defaults::minCollumnWidth);
    state.tilesetWidth = std::max(state.tilesetWidth, defaults::minCollumnWidth);

    int middle = state.windowWidth - state.layerWidth - state.tilesetWidth;
    if (middle < defaults::minCollumnWidth)
    {
        // Fix by shrinking tileset first
        int shrink = defaults::minCollumnWidth - middle;
        state.tilesetWidth = std::max(defaults::minCollumnWidth, state.tilesetWidth - shrink);
    }    

    // Toolbar y
    int hTop = (int)SendMessage(context.hRebarTop, RB_GETBARHEIGHT, 0, 0);        
    int hBottom = (int)SendMessage(context.hRebarBottom, RB_GETBARHEIGHT, 0, 0);

    state.toolbarOffset = hTop + hBottom;

    int splitW = 5;
    int splitH = 5;

    hdwp = BeginDeferWindowPos(1);    
    hdwp = defer(hdwp, context.hRebarBottom, 0, hTop, rc.right, hBottom);
    EndDeferWindowPos(hdwp);
    
    hdwp = BeginDeferWindowPos(7);

    hdwp = defer(hdwp, context.hLayerListView, 0, state.toolbarOffset, state.layerWidth, state.layerHeight);
    hdwp = defer(hdwp, context.hSplitBottom, 0, state.toolbarOffset + state.layerHeight, state.layerWidth, splitH);
    hdwp = defer(hdwp, context.hPackageView, 0, state.toolbarOffset + state.layerHeight + splitH, state.layerWidth, rc.bottom - state.toolbarOffset - state.layerHeight - splitH);
    hdwp = defer(hdwp, context.hSplitLeft, state.layerWidth, state.toolbarOffset, splitW, rc.bottom - state.toolbarOffset);
    hdwp = defer(hdwp, context.hMapView, state.layerWidth + splitW, state.toolbarOffset, rc.right - state.layerWidth - state.tilesetWidth - 2*splitW, rc.bottom - state.toolbarOffset);
    hdwp = defer(hdwp, context.hSplitRight, rc.right - state.tilesetWidth - splitW, state.toolbarOffset, splitW, rc.bottom - state.toolbarOffset);
    hdwp = defer(hdwp, context.hTilesetView, rc.right - state.tilesetWidth, state    .toolbarOffset, state.tilesetWidth, rc.bottom - state.toolbarOffset);

    EndDeferWindowPos(hdwp);

    state.toolbarOffset = hTop + hBottom;
}

void win32_program::LayoutContent(HWND hwnd, Win32Context& context)
{
    auto defer = [](HDWP dwp, HWND handle, int x, int y, int w, int h){
        return DeferWindowPos(
            dwp, handle, nullptr,
            x, y, w, h,
            SWP_NOZORDER
        );
    };

    RECT rc;
    GetClientRect(hwnd, &rc);

    auto& state = GetSectionState();
    int splitW = 5;
    int splitH = 5;

    HDWP hdwp = BeginDeferWindowPos(8);

    hdwp = defer(hdwp, context.hLayerListView, 0, state.toolbarOffset, state.layerWidth, state.layerHeight);
    hdwp = defer(hdwp, context.hSplitBottom, 0, state.toolbarOffset + state.layerHeight, state.layerWidth, splitH);
    hdwp = defer(hdwp, context.hPackageView, 0, state.toolbarOffset + state.layerHeight + splitH, state.layerWidth, rc.bottom - state.toolbarOffset - state.layerHeight - splitH);
    hdwp = defer(hdwp, context.hSplitLeft, state.layerWidth, state.toolbarOffset, splitW, rc.bottom - state.toolbarOffset);
    hdwp = defer(hdwp, context.hMapView, state.layerWidth + splitW, state.toolbarOffset, rc.right - state.layerWidth - state.tilesetWidth - 2*splitW, rc.bottom - state.toolbarOffset);
    hdwp = defer(hdwp, context.hSplitRight, rc.right - state.tilesetWidth - splitW, state.toolbarOffset, splitW, rc.bottom - state.toolbarOffset);

    hdwp = defer(hdwp, context.hTilesetView, rc.right - state.tilesetWidth, state.toolbarOffset, state.tilesetWidth, rc.bottom - state.toolbarOffset);

    EndDeferWindowPos(hdwp);
}

