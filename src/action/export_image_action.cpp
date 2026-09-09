#include "action/export_image_action.hpp"
#include "program/program.hpp"
#include "program/except.hpp"
#include "win32_helpers/file_helpers.hpp"

action::ExportImageAction::ExportImageAction() 
{
    m_actionType = ActionType::ExportAsImage;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = 510;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::Export;
    m_actionDescription.menuId = MenuId::None;
    m_actionDescription.tooltipStringId = std::nullopt;
    m_actionDescription.nameStringId = locale::StringId::NameExportAsImage;
    m_actionDescription.shortcutStringId = locale::StringId::ShortcutNameExportAsImage;
}

void action::ExportImageAction::Execute(program::ProgramContext& context)
{
    auto mapDocument = context.fileManager->GetActiveDocument();

    if(mapDocument == nullptr) {
        return;
    }

    auto imageFileName = context.stringLookup.Get(locale::StringId::NameImageFile);
    auto allFilesString = context.stringLookup.Get(locale::StringId::NameAllFiles);
    auto acceptableImageExtensions = std::vector<std::wstring>{ L"png", L"jpg", L"jpeg", L"bmp" };

    if(!imageFileName.has_value() || !allFilesString.has_value()) {
        throw program::StringNotFoundException("String not found for AllFiles or NameImageFile.");
    }

    auto filePath = win32_helpers::ShowSaveDialog(
        context.mainWindowContext->hMainWindow,
        {
            { imageFileName.value().c_str(), acceptableImageExtensions },
            { allFilesString.value().c_str(), { L"*" } }
        }
    );

    if(!filePath.has_value()) {
        return;
    }

    context.mapSection->RenderToImage(filePath.value());
}