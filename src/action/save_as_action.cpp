#include "action/save_as_action.hpp"
#include "program/program.hpp"
#include "program/except.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "locale/stringid.hpp"
#include "file/ifile.hpp"

action::SaveAsAction::SaveAsAction()
{
    m_actionType = ActionType::SaveAs;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = 310;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::File;
    m_actionDescription.menuId = MenuId::File;
    m_actionDescription.tooltipStringId = std::nullopt;
    m_actionDescription.nameStringId = locale::StringId::NameSaveAs;    
}

void SaveAndUpdate(file::IFile* activeFile, std::filesystem::path filePath, program::ProgramContext& programContext)
{
    activeFile->SetFilePath(filePath);

    auto fileManager = programContext.GetManager<file::FileManager>();
    fileManager->SaveFile(
        fileManager->GetSelectedFileIndex()
    );

    auto selectedDocument = fileManager->GetSelectedDocumentLocation();
    auto activeDocument = fileManager->GetActiveDocumentLocation();

    fileManager->SelectDocument(selectedDocument);
    fileManager->SetActiveDocument(activeDocument);

    auto packageSection = programContext.GetSection<sections::PackageSection>();
    packageSection->Refresh(programContext);
    packageSection->UpdateTreeViewItems(programContext);
    packageSection->Update();
}

void action::SaveAsAction::Execute(program::ProgramContext& programContext)
{       
    static std::unordered_map<file::FileType, locale::StringId> extensionToStringIdMap = {
        { file::FileType::Map, locale::StringId::NameMapFile },
        { file::FileType::Tileset, locale::StringId::NameTilesetFile },
        { file::FileType::Package, locale::StringId::NamePackageFile }
    };

    auto activeFile = programContext.GetManager<file::FileManager>()->GetSelectedFile();
    auto stringLookup = programContext.GetStringLookup();
    
    if (activeFile != nullptr) 
    {
        auto allFilesString = stringLookup.Get(locale::StringId::NameAllFiles);
        auto thisTypeString = stringLookup.Get(
            extensionToStringIdMap[activeFile->GetFileType()]
        );

        if(!thisTypeString.has_value() || !allFilesString.has_value()) {
            throw program::StringNotFoundException("Missing string for file type or all files");
        }

        std::wstring thisTypeStr = std::format(
            L"{} ({})",
            thisTypeString.value(),
            activeFile->GetExtension()
        );

        std::optional<std::filesystem::path> filePath = win32_helpers::ShowSaveDialog(
            programContext.GetMainWindowHandle(),
            {
                { thisTypeStr.c_str(), { activeFile->GetExtension().c_str() } },
                { allFilesString.value().c_str(), { L"*" } }
            }
        );

        if(filePath.has_value())
        {
            SaveAndUpdate(activeFile, filePath.value(), programContext);
        }
    }
}
