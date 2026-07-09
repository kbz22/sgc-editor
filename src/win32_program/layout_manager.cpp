#include "win32_program/layout_manager.hpp"
#include <windowsx.h>
#include <commctrl.h>
#include <algorithm>

win32_program::LayoutManager::LayoutManager(program::ProgramContext& programContext)
    : m_programContext(programContext)
{
    auto makeSplitter = [&](win32_program::ControlId id)
    {
        return CreateWindowEx(
            0, L"STATIC", nullptr,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
            0,0,0,0,
            programContext.MainWindowContext->hMainWindow, (HMENU)id, programContext.MainWindowContext->hInstance, nullptr
        );
    };

    m_layerPackageSplitter = makeSplitter(win32_program::ControlId::SplitLayerPackage);
    m_tilesetMapSplitter = makeSplitter(win32_program::ControlId::SplitTilesetMap);
    m_layerMapSplitter = makeSplitter(win32_program::ControlId::SplitLayerMap);

    RECT rc;
    GetClientRect(programContext.MainWindowContext->hMainWindow, &rc);

    m_windowWidth  = rc.right;
    m_windowHeight = rc.bottom;
}

void win32_program::LayoutManager::HandleResize(HWND hwnd, LPARAM lParam)
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

    auto hRebarTop = m_programContext.menuSection->GetHwnd();
    auto hRebarBottom = m_programContext.toolbarSection->GetHwnd();

    int hTop = static_cast<int>(
        SendMessage(hRebarTop, RB_GETBARHEIGHT, 0, 0)
    );
    int hBottom = static_cast<int>(
        SendMessage(hRebarBottom, RB_GETBARHEIGHT, 0, 0)
    );

    m_toolbarOffset = hTop + hBottom;

    HDWP hdwp = BeginDeferWindowPos(2);
    hdwp = DeferWindowPos(hdwp, hRebarTop,    nullptr,
                        0, 0, rc.right, 0,
                        SWP_NOZORDER);
    hdwp = DeferWindowPos(hdwp, hRebarBottom, nullptr,
                        0, 0, rc.right, 0,
                        SWP_NOZORDER);
    EndDeferWindowPos(hdwp);    

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

    hdwp = BeginDeferWindowPos(1);    
    hdwp = defer(hdwp, hRebarBottom, 0, hTop, rc.right, hBottom);
    EndDeferWindowPos(hdwp);
    
    hdwp = BeginDeferWindowPos(7);

    hdwp = defer(hdwp, m_programContext.layersSection->GetHwnd(), 0, m_toolbarOffset, layerWidth, layerHeight);
    hdwp = defer(hdwp, m_layerPackageSplitter, 0, m_toolbarOffset + layerHeight, layerWidth, m_splitH);
    hdwp = defer(hdwp, m_programContext.packageSection->GetHwnd(), 0, m_toolbarOffset + layerHeight + m_splitH, layerWidth, rc.bottom - m_toolbarOffset - layerHeight - m_splitH);
    hdwp = defer(hdwp, m_layerMapSplitter, layerWidth, m_toolbarOffset, m_splitW, rc.bottom - m_toolbarOffset);
    hdwp = defer(hdwp, m_programContext.mapSection->GetHwnd(), layerWidth + m_splitW, m_toolbarOffset, rc.right - layerWidth - tilesetWidth - 2*m_splitW, rc.bottom - m_toolbarOffset);
    hdwp = defer(hdwp, m_tilesetMapSplitter, rc.right - tilesetWidth - m_splitW, m_toolbarOffset, m_splitW, rc.bottom - m_toolbarOffset);
    hdwp = defer(hdwp, m_programContext.tilesetSection->GetHwnd(), rc.right - tilesetWidth, m_toolbarOffset, tilesetWidth, rc.bottom - m_toolbarOffset);

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

void win32_program::LayoutManager::HandleDragging(HWND hwnd, LPARAM lParam)
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