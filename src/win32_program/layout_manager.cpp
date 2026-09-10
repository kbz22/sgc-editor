#include "win32_program/layout_manager.hpp"
#include <windowsx.h>
#include <commctrl.h>
#include <algorithm>

win32_program::LayoutManager::LayoutManager(program::ProgramContext& programContext)
    : m_programContext(programContext)
{
    auto hMainWindow = programContext.GetMainWindowHandle();
    auto hInstance = programContext.GetHInstance();
    auto makeSplitter = [&](win32_program::ControlId id)
    {
        return CreateWindowEx(
            0, L"STATIC", nullptr,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
            0,0,0,0,
            hMainWindow,
            (HMENU)id,
            hInstance,
            nullptr
        );
    };

    m_layerPackageSplitter = makeSplitter(win32_program::ControlId::SplitLayerPackage);
    m_tilesetMapSplitter = makeSplitter(win32_program::ControlId::SplitTilesetMap);
    m_layerMapSplitter = makeSplitter(win32_program::ControlId::SplitLayerMap);

    RECT rc;
    GetClientRect(hMainWindow, &rc);

    m_windowWidth  = rc.right;
    m_windowHeight = rc.bottom;

    auto hRebarTop = m_programContext.GetSection<sections::MenuSection>()->GetHwnd();
    auto hRebarBottom = m_programContext.GetSection<sections::ToolbarSection>()->GetHwnd();
    
    int hTop = static_cast<int>(
        SendMessage(hRebarTop, RB_GETBARHEIGHT, 0, 0)
    );
    int hBottom = static_cast<int>(
        SendMessage(hRebarBottom, RB_GETBARHEIGHT, 0, 0)
    );

    m_menuBarHeight = hTop;
    m_toolbarHeight = hBottom;
}

// Rebar heights are queried once during initialization.
//
// Querying RB_GETBARHEIGHT during every layout caused intermittent
// overlap/flickering while resizing because the rebar performs its
// own internal layout asynchronously.
//
// These controls have fixed heights in this application, so caching
// them avoids fighting the rebar's layout logic.
// chat gpt wrote this for me isn't he nice :)

void win32_program::LayoutManager::HandleResize(HWND hwnd, [[maybe_unused]] LPARAM lParam)
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

    auto menuSection = m_programContext.GetSection<sections::MenuSection>();
    auto toolbarSection = m_programContext.GetSection<sections::ToolbarSection>();
    auto layersSection = m_programContext.GetSection<sections::LayersSection>();
    auto packageSection = m_programContext.GetSection<sections::PackageSection>();
    auto mapSection = m_programContext.GetSection<sections::MapSection>();
    auto tilesetSection = m_programContext.GetSection<sections::TilesetSection>();
    auto statusSection = m_programContext.GetSection<sections::StatusSection>();

    auto hRebarTop = menuSection->GetHwnd();
    auto hRebarBottom = toolbarSection->GetHwnd();  
    
    UpdateRebarLayout(hwnd, rc.right); // this is important to do before relaying the sections

    int hTop = m_menuBarHeight;
    int hBottom = m_toolbarHeight;
    
    m_toolbarOffset = hTop + hBottom;

    m_windowWidth = rc.right;
    m_windowHeight = rc.bottom;

    const int clientHeight = m_windowHeight - m_toolbarOffset;

    auto layerWidth = static_cast<int>(m_windowWidth * m_layersPackageViewRatio);
    auto tilesetWidth = static_cast<int>(m_windowWidth * m_tilesetMapRatio);
    auto layerHeight = static_cast<int>(clientHeight * m_layersMapViewRatio);

    layerWidth = std::max(layerWidth, defaults::minCollumnWidth);
    tilesetWidth = std::max(tilesetWidth, defaults::minCollumnWidth);

    auto middle = m_windowWidth - layerWidth - tilesetWidth;
    if (middle < defaults::minCollumnWidth)
    {
        // Fix by shrinking tileset first
        int shrink = defaults::minCollumnWidth - middle;
        tilesetWidth = std::max(defaults::minCollumnWidth, tilesetWidth - shrink);
    }    

    RECT statusBarRect;
    GetClientRect(statusSection->GetHwnd(), &statusBarRect);
    m_statusBarHeight = statusBarRect.bottom - statusBarRect.top;

    HDWP hdwp; 
    
    hdwp = BeginDeferWindowPos(10);

    hdwp = defer(hdwp, hRebarTop, 0, 0, rc.right, hTop);
    hdwp = defer(hdwp, hRebarBottom, 0, hTop, rc.right, hBottom);
    hdwp = defer(hdwp, layersSection->GetHwnd(), 0, m_toolbarOffset, layerWidth, layerHeight);
    hdwp = defer(hdwp, m_layerPackageSplitter, 0, m_toolbarOffset + layerHeight, layerWidth, m_splitH - m_statusBarHeight);
    hdwp = defer(hdwp, packageSection->GetHwnd(), 0, m_toolbarOffset + layerHeight + m_splitH, layerWidth, rc.bottom - m_toolbarOffset - layerHeight - m_splitH - m_statusBarHeight);
    hdwp = defer(hdwp, m_layerMapSplitter, layerWidth, m_toolbarOffset, m_splitW, rc.bottom - m_toolbarOffset - m_statusBarHeight);
    hdwp = defer(hdwp, mapSection->GetHwnd(), layerWidth + m_splitW, m_toolbarOffset, rc.right - layerWidth - tilesetWidth - 2*m_splitW, rc.bottom - m_toolbarOffset - m_statusBarHeight);
    hdwp = defer(hdwp, m_tilesetMapSplitter, rc.right - tilesetWidth - m_splitW, m_toolbarOffset, m_splitW, rc.bottom - m_toolbarOffset - m_statusBarHeight);
    hdwp = defer(hdwp, tilesetSection->GetHwnd(), rc.right - tilesetWidth, m_toolbarOffset, tilesetWidth, rc.bottom - m_toolbarOffset - m_statusBarHeight);
    hdwp = defer(hdwp, statusSection->GetHwnd(), 0, rc.bottom - m_statusBarHeight, rc.right, m_statusBarHeight);

    EndDeferWindowPos(hdwp);
}

win32_program::DraggedSplitter win32_program::LayoutManager::GetDraggedSplitter(HWND hwnd, LPARAM lParam)
{
    HWND child = ChildWindowFromPoint(hwnd, { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });    

    if (child == m_layerPackageSplitter)
    {
        m_draggedSplitter = DraggedSplitter::LayerPackage;
    }
    else if (child == m_tilesetMapSplitter)
    {
        m_draggedSplitter = DraggedSplitter::TilesetMap;        
    }
    else if (child == m_layerMapSplitter)
    {
        m_draggedSplitter = DraggedSplitter::LayerMap;        
    }

    return m_draggedSplitter;
}

void win32_program::LayoutManager::ResetDraggedSplitter()
{
    m_draggedSplitter = DraggedSplitter::None;
}

void win32_program::LayoutManager::HandleDragging([[maybe_unused]] HWND hwnd, LPARAM lParam)
{
    const int x = GET_X_LPARAM(lParam);
    const int y = GET_Y_LPARAM(lParam);

    const int clientWidth  = m_windowWidth;
    const int clientHeight = m_windowHeight - m_toolbarOffset;

    int leftWidth = int(clientWidth  * m_layersPackageViewRatio);
    int tilesetWidth = int(clientWidth  * m_tilesetMapRatio);
    int topHeight = int(clientHeight * m_layersMapViewRatio);

    switch (m_draggedSplitter)
    {
    case DraggedSplitter::LayerPackage:
    {
        topHeight = std::clamp(
            y - m_toolbarOffset,
            defaults::minCollumnHeight,
            clientHeight - defaults::minCollumnHeight
        );
        break;
    }

    case DraggedSplitter::LayerMap:
    {
        leftWidth = std::clamp(
            x,
            defaults::minCollumnWidth,
            clientWidth - tilesetWidth - 2 * defaults::minCollumnWidth
        );
        break;
    }

    case DraggedSplitter::TilesetMap:
    {
        tilesetWidth = std::clamp(
            clientWidth - x - m_splitW,
            defaults::minCollumnWidth,
            clientWidth - leftWidth - 2 * defaults::minCollumnWidth
        );
        break;
    }

    default:
        return;
    }

    m_layersPackageViewRatio = static_cast<float>(leftWidth) / clientWidth;
    m_tilesetMapRatio = static_cast<float>(tilesetWidth) / clientWidth;
    m_layersMapViewRatio = static_cast<float>(topHeight) / clientHeight;
}

void win32_program::LayoutManager::UpdateRebarLayout([[maybe_unused]] HWND hwnd, int width)
{
    HDWP hdwp = BeginDeferWindowPos(2);

    hdwp = DeferWindowPos(
        hdwp,
        m_programContext.GetSection<sections::MenuSection>()->GetHwnd(),
        nullptr,
        0, 0,
        width,
        m_menuBarHeight,
        SWP_NOZORDER
    );

    hdwp = DeferWindowPos(
        hdwp,
        m_programContext.GetSection<sections::ToolbarSection>()->GetHwnd(),
        nullptr,
        0, m_menuBarHeight,
        width,
        m_toolbarHeight,
        SWP_NOZORDER
    );

    EndDeferWindowPos(hdwp);
}

SIZE win32_program::LayoutManager::GetMinimumSize() const
{
    SIZE minSize;
    minSize.cx = 3 * defaults::minCollumnWidth + 2 * m_splitW;
    minSize.cy = 2 * defaults::minCollumnHeight + m_splitH + m_menuBarHeight + m_toolbarHeight + m_statusBarHeight;

    return minSize;
}