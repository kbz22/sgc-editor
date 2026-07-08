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

    HDWP hdwp = BeginDeferWindowPos(2);
    hdwp = DeferWindowPos(hdwp, hRebarTop,    nullptr,
                        0, 0, rc.right, 0,
                        SWP_NOZORDER);
    hdwp = DeferWindowPos(hdwp, hRebarBottom, nullptr,
                        0, 0, rc.right, 0,
                        SWP_NOZORDER);
    EndDeferWindowPos(hdwp);    

    auto windowWidth = rc.right;
    auto windowHeight = rc.bottom;

    auto layerWidth = static_cast<int>(windowWidth * m_layersPackageViewRatio);
    auto tilesetWidth = static_cast<int>(windowWidth * m_tilesetMapRatio);
    auto layerHeight = static_cast<int>(windowHeight * m_layersMapViewRatio);

    layerWidth = std::max(layerWidth, defaults::minCollumnWidth);
    tilesetWidth = std::max(tilesetWidth, defaults::minCollumnWidth);

    auto middle = windowWidth - layerWidth - tilesetWidth;
    if (middle < defaults::minCollumnWidth)
    {
        // Fix by shrinking tileset first
        int shrink = defaults::minCollumnWidth - middle;
        tilesetWidth = std::max(defaults::minCollumnWidth, tilesetWidth - shrink);
    }

    int hTop = static_cast<int>(
        SendMessage(hRebarTop, RB_GETBARHEIGHT, 0, 0)
    );
    int hBottom = static_cast<int>(
        SendMessage(hRebarBottom, RB_GETBARHEIGHT, 0, 0)
    );

    m_toolbarOffset = hTop + hBottom;

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