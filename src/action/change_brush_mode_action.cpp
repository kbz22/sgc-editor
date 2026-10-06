#include "action/change_brush_mode_action.hpp"
#include "program/program.hpp"


action::ChangeBrushModeAction::ChangeBrushModeAction(editor_tools::PaintMode brushMode) :
    m_brushMode(brushMode)
{
    switch(m_brushMode) {

        case editor_tools::PaintMode::Brush:
        {
            m_actionType = ActionType::PaintModeBrush;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorPaintModeBrush;
            m_actionDescription.nameStringId = locale::StringId::NamePaintModeBrush;
            m_actionDescription.imageIndex = 19;
            m_actionDescription.toolbarOrder = 1500;
            m_actionDescription.menuOrder = 800;
            m_actionDescription.shortcuts = {
                { win32_program::ShortcutModifier::None, 'B' }
            };
            break;
        }

        case editor_tools::PaintMode::Rectangle:
        {
            m_actionType = ActionType::PaintModeRectangle;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorPaintModeRectangle;
            m_actionDescription.nameStringId = locale::StringId::NamePaintModeRectangle;
            m_actionDescription.imageIndex = 20;
            m_actionDescription.toolbarOrder = 1501;
            m_actionDescription.menuOrder = 801;
            m_actionDescription.shortcuts = {
                { win32_program::ShortcutModifier::None, 'R' }
            };
            break;
        }

        case editor_tools::PaintMode::Fill:
        {
            m_actionType = ActionType::PaintModeFill;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorPaintModeFill;
            m_actionDescription.nameStringId = locale::StringId::NamePaintModeFill;
            m_actionDescription.imageIndex = 22;
            m_actionDescription.toolbarOrder = 1502;
            m_actionDescription.menuOrder = 802;
            m_actionDescription.shortcuts = {
                { win32_program::ShortcutModifier::None, 'F' }
            };
            break;
        }

        case editor_tools::PaintMode::Select:
        {
            m_actionType = ActionType::PaintModeSelect;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorPaintModeSelect;
            m_actionDescription.nameStringId = locale::StringId::NamePaintModeSelect;
            m_actionDescription.imageIndex = 24;
            m_actionDescription.toolbarOrder = 1503;
            m_actionDescription.menuOrder = 803;
            m_actionDescription.shortcuts = {
                { win32_program::ShortcutModifier::None, 'S' }
            };
            break;
        }        

        case editor_tools::PaintMode::TilePicker:
        {
            m_actionType = ActionType::TilePickerTool;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipTilePicker;
            m_actionDescription.nameStringId = locale::StringId::NameTilePicker;
            m_actionDescription.imageIndex = 36;
            m_actionDescription.toolbarOrder = 1504;
            m_actionDescription.menuOrder = 804;
            m_actionDescription.shortcuts = {
                {win32_program::ShortcutModifier::Ctrl, win32_program::ShortcutKey::None}
            };
            break;
        }

        case editor_tools::PaintMode::ChunkRemover:
        {
            m_actionType = ActionType::ChunkRemoverTool;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorEraseModeDeleteChunk;
            m_actionDescription.nameStringId = locale::StringId::NameEraserModeDeleteChunk;
            m_actionDescription.imageIndex = 38;
            m_actionDescription.toolbarOrder = 1505;
            m_actionDescription.menuOrder = 805;
            m_actionDescription.shortcuts = {
                {win32_program::ShortcutModifier::Ctrl, 'D'},
                {win32_program::ShortcutModifier::Shift, win32_program::ShortcutKey::Delete},
            };
            break;
        }
    }

    m_enabled = true;
    m_checked = false;

    m_actionDescription.checkGroupItem = true;
    m_actionDescription.groupId = GroupId::BrushMode;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::MapEditor;
}

void action::ChangeBrushModeAction::Execute([[maybe_unused]] program::ProgramContext& context)
{    
    auto mapSection = context.GetSection<sections::MapSection>();
    auto currentSelectionMode = mapSection->GetSelectionMode();

    if(m_brushMode != editor_tools::PaintMode::Select) {
        mapSection->ResetSelection();
    }

    context.UpdateBrushMode(m_brushMode);
    context.UpdateEditorSelectionMode(currentSelectionMode);
    context.UpdateEditorSelectionTools();
}