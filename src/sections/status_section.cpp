#include "sections/status_section.hpp"
#include "program/program.hpp"
#include <vector>

sections::StatusSection::StatusSection(program::ProgramContext& programContext)
{
    auto parentHwnd = programContext.mainWindowContext->hMainWindow;
    auto hwnd = CreateWindowEx(
        0,
        STATUSCLASSNAME,
        nullptr,
        WS_CHILD | WS_VISIBLE,
        0, 0, 0, 0,
        parentHwnd,
        nullptr,
        programContext.mainWindowContext->hInstance,
        nullptr
    );

    SetHwnd(hwnd, parentHwnd);

    SendMessage(
        hwnd,
        SB_SETPARTS,
        3,
        reinterpret_cast<LPARAM>(m_partsWidths)
    );

    for(int i=0; i < static_cast<int>(StatusField::Count); ++i)
    {        
        SendMessage(
            hwnd,
            SB_SETTEXT,
            i,
            reinterpret_cast<LPARAM>(L"")
        );
    }

}

sections::StatusSection::~StatusSection()
{
    return;
}

void sections::StatusSection::Update()
{
    Redraw();
    return;
}

void sections::StatusSection::Refresh([[maybe_unused]] program::ProgramContext& programContext)
{
    return;
}

void sections::StatusSection::HandleSectionResize()
{
    return;
}

void sections::StatusSection::SetStatusText(StatusField field, const std::wstring& text)
{
    m_statusTexts[field] = m_padding + text;
    SendMessage(
        GetHwnd(),
        SB_SETTEXT,
        static_cast<WPARAM>(field),
        reinterpret_cast<LPARAM>(m_statusTexts[field].c_str())
    );
}

void sections::StatusSection::SetStatusCursorPosition(const sgc::graphics::PixelPosition2D& position)
{
    std::wstring text = L"Cursor: (" + std::to_wstring(position.x) + L", " + std::to_wstring(position.y) + L")";
    SetStatusText(StatusField::CursorPosition, text);
}

void sections::StatusSection::SetStatusTileId(std::optional<sgc::tile::TileId> tileId)
{
    std::wstring tileIdText = (tileId.has_value()) ? std::to_wstring(tileId.value()) : L"None";
    std::wstring text = L"Tile ID: " + tileIdText;
    SetStatusText(StatusField::TileId, text);
}

void sections::StatusSection::SetStatusTileId(std::wstring tileIdText)
{
    std::wstring text = L"Tile ID: " + tileIdText;
    SetStatusText(StatusField::TileId, text);
}

void sections::StatusSection::SetStatusSelectionSize(const sgc::math::vec2& size)
{
    std::wstring text = L"Selection: [" + std::to_wstring(size.x) + L", " + std::to_wstring(size.y) + L"]";
    SetStatusText(StatusField::SelectionSize, text);
}