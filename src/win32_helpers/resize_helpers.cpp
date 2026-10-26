#include "win32_helpers/resize_helpers.hpp"

void win32_helpers::ResizeScrollBarsForImage(HWND hwnd, int imageWidth, int imageHeight)
{
    RECT rect{};
    GetClientRect(hwnd, &rect);

    SCROLLINFO scrollInfo{};
    scrollInfo.cbSize = sizeof(scrollInfo);
    scrollInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

    // horizontal
    scrollInfo.nMin = 0;
    scrollInfo.nMax = imageWidth - 1;
    scrollInfo.nPage = rect.right - rect.left;
    scrollInfo.nPos = 0;

    SetScrollInfo(hwnd, SB_HORZ, &scrollInfo, TRUE);

    // vertical
    scrollInfo.nMin = 0;
    scrollInfo.nMax = imageHeight - 1;
    scrollInfo.nPage = rect.bottom - rect.top;
    scrollInfo.nPos = 0;

    SetScrollInfo(hwnd, SB_VERT, &scrollInfo, TRUE);
}