#include "win32_program/toolbar_functions.hpp"

#include "sgc_view/sgc_view.hpp"
#include "win32_program/windows_init.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "program/program.hpp"
#include "program/except.hpp"
#include "defaults.hpp"

#include <sgc/asset/chunkedtilestorageserializer.hpp> //! tmp testing serialization
#include <sgc/asset/chunkedtilestorageassetbuilder.hpp>
#include <sgc/asset/chunkedtilestorageloader.hpp>
#include <sgc/data/resourcemanager.hpp>

#include <fstream>
#include <vector>

#include "new_file_dialog.h"
#include <commdlg.h>

INT_PTR NewFileDialogCommandHandler(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    (void)msg;
    (void)lParam;

    switch (LOWORD(wParam))
    {

        case IDC_BROWSE_BUTTON:
        {
            wchar_t filePath[MAX_PATH] = {0};

            OPENFILENAME ofn = {};
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hDlg;
            ofn.lpstrFilter = L"Image Files\0*.png;*.bmp;*.jpg;*.jpeg\0All Files\0*.*\0";
            ofn.lpstrFile = filePath;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

            if (GetOpenFileName(&ofn))
            {
                // Put the selected path into the text field
                SetDlgItemText(hDlg, IDC_PATH_EDIT, filePath);
            }
            return TRUE;
        }

        case IDOK:
        {
            wchar_t buffer[MAX_PATH];
            GetDlgItemText(hDlg, IDC_PATH_EDIT, buffer, MAX_PATH);
            
            try
            {
                program::StartEditor(
                    buffer,
                    GetDlgItemInt(hDlg, IDC_MAP_WIDTH, nullptr, FALSE),
                    GetDlgItemInt(hDlg, IDC_MAP_HEIGHT, nullptr, FALSE),
                    1,
                    1
                );
            }
            catch (const program::TileSizeException& e)
            {
                MessageBoxA(hDlg, e.what(), "Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }

            EndDialog(hDlg, IDOK);
            return TRUE;
        }


        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
    }
    
    return FALSE;
}

INT_PTR CALLBACK NewFileDialogProc([[maybe_unused]] HWND hDlg, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    switch (msg)
    {
        case WM_INITDIALOG:
        {
            SetDlgItemInt(hDlg, IDC_MAP_WIDTH, defaults::tileSize, FALSE);
            SetDlgItemInt(hDlg, IDC_MAP_HEIGHT, defaults::tileSize, FALSE);
            return TRUE;
        }
        
        case WM_COMMAND:
        {
            return NewFileDialogCommandHandler(hDlg, msg, wParam, lParam);        
        }
    }

    return FALSE;
}


void win32_program::OnFileNewClicked()
{    
    auto& programContext = program::GetProgramContext();

    // if (context.hTilesetView != nullptr) {
    if(programContext.tilesetSection != nullptr) {

        if(programContext.mainWindowContext->hMainWindow != nullptr)
        DialogBox(
            programContext.mainWindowContext->hInstance,            
            MAKEINTRESOURCE(IDD_NEWFILE_DIALOG),
            programContext.mainWindowContext->hMainWindow,
            NewFileDialogProc
        );
    }
}

void win32_program::OnFileSaveClicked()
{
    using namespace sgc;

    auto& contextWin32 = GetMainWindowContext();
    auto& contextProgram = program::GetProgramContext();    
    
    /* auto storage = contextProgram.mapView->GetStorage();
    auto asset = asset::AssetBuilder<asset::ChunkedTileStorageAsset>::Build(*storage);   */  

    // auto bytes = asset::AssetSerializer<asset::ChunkedTileStorageAsset>::Serialize(asset);

    win32_helpers::FileFilter mapFilter{
        L"Map Files",
        {L"sgcmap"}
    };

    auto path = win32_helpers::ShowSaveDialog(contextWin32.hMainWindow, {mapFilter});
    if (!path) return;

    contextProgram.mapSection->SaveMap(*path);

   /*  std::ofstream file(*path, std::ios::binary);
    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size()); */

}

void win32_program::OnFileOpenClicked()
{
    auto& contextWin32 = GetMainWindowContext();
    auto& contextProgram = program::GetProgramContext();

    win32_helpers::FileFilter allFilter{
        L"All Files",
        {L"*"}
    };

    auto path = win32_helpers::ShowOpenDialog(contextWin32.hMainWindow, {allFilter});
    if (!path) return;

    contextProgram.mapSection->LoadMap(*path);

    /* std::ifstream file(*path, std::ios::binary);
    std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    sgc::data::ResourceContext context;
    sgc::data::ResourceManager rm;

    auto storage = sgc::asset::AssetLoader<sgc::data::ChunkedTileStorage>::Load(
        bytes,
        context,
        rm
    ); */

    // contextProgram.mapView->SetStorage(storage);
}

void win32_program::OnEditUndoClicked()
{
    auto& programContext = program::GetProgramContext();

    if(programContext.commandManager != nullptr) {
        programContext.commandManager->Undo();
    }

    programContext.mapSection->Update();
}

void win32_program::OnEditRedoClicked()
{
    auto& programContext = program::GetProgramContext();

    if(programContext.commandManager != nullptr) {
        programContext.commandManager->Redo();
    }

    programContext.mapSection->Update();
}

void win32_program::OnLayerAddClicked()
{
    auto& programContext = program::GetProgramContext();

    if(programContext.layerManager != nullptr) {

        auto storage = std::make_shared<sgc::data::ChunkedTileStorage>();
        storage->SetTileAt({0, 0}, 0);        

        programContext.layerManager->AddLayer({
            storage,
            L"New Layer"
        });

        UpdateEditorLayerMode(programContext.editorLayerMode);

        programContext.mapSection->Refresh(*programContext.layerManager);
        programContext.mapSection->Update();

        programContext.layersSection->Refresh(*programContext.layerManager);
        programContext.layersSection->Update();
    }
}

void win32_program::OnLayerRemoveClicked()
{
    auto& programContext = program::GetProgramContext();

    if(programContext.layerManager != nullptr) {
        auto activeIndex = programContext.layerManager->GetActiveLayerIndex();
        programContext.layerManager->RemoveLayer(activeIndex);

        UpdateEditorLayerMode(programContext.editorLayerMode);

        programContext.mapSection->Refresh(*programContext.layerManager);
        programContext.mapSection->Update();

        programContext.layersSection->Refresh(*programContext.layerManager);
        programContext.layersSection->Update();
    }
}

void win32_program::OnLayerMoveUpClicked()
{
    auto& programContext = program::GetProgramContext();

    if(programContext.layerManager != nullptr) {
        try {
            // programContext.layerManager->MoveLayer(programContext.layerManager->GetActiveLayerIndex(), -1);
            programContext.layerManager->MoveActiveLayer(-1);
        } catch ([[maybe_unused]]const std::out_of_range& e) {
            //! no need to note the out of range error
        }
        
        UpdateEditorLayerMode(programContext.editorLayerMode);

        programContext.mapSection->Refresh(*programContext.layerManager);
        programContext.mapSection->Update();

        programContext.layersSection->SetSelectedLayer(programContext.layerManager->GetActiveLayerIndex());
        programContext.layersSection->Refresh(*programContext.layerManager);
        programContext.layersSection->Update();
    }
}

void win32_program::OnLayerMoveDownClicked()
{
    auto& programContext = program::GetProgramContext();

    if(programContext.layerManager != nullptr) {
        try {
            // programContext.layerManager->MoveLayer(programContext.layerManager->GetActiveLayerIndex(), 1);
            programContext.layerManager->MoveActiveLayer(1);
        } catch ([[maybe_unused]]const std::out_of_range& e) {
            //! no need to note the out of range error
        }

        UpdateEditorLayerMode(programContext.editorLayerMode);

        programContext.mapSection->Refresh(*programContext.layerManager);
        programContext.mapSection->Update();

        programContext.layersSection->SetSelectedLayer(programContext.layerManager->GetActiveLayerIndex());
        programContext.layersSection->Refresh(*programContext.layerManager);
        programContext.layersSection->Update();
    }
}

void win32_program::UpdateEditorLayerMode(program::EditorLayerMode newMode)
{
    auto& programContext = program::GetProgramContext();

    programContext.editorLayerMode = newMode;
    
    switch(newMode) {

        case program::EditorLayerMode::SingleLayer:
        {
            programContext.layerManager->SetSingleLayerMode(true);            
            break;
        }

        case program::EditorLayerMode::MultiLayer:
        {
            programContext.layerManager->SetSingleLayerMode(false);
            program::MultiLayerModeSetup(programContext);
            break;
        }

        case program::EditorLayerMode::SingleImage:
        {
            programContext.layerManager->SetSingleLayerMode(false);
            program::SingleImageModeSetup(programContext);
            break;
        }
    }

    programContext.mapSection->Refresh(*programContext.layerManager);
    programContext.mapSection->Update();
}

void win32_program::UpdateEditorChunkMode(program::EditorChunkMode newMode)
{
    auto& programContext = program::GetProgramContext();

    programContext.editorChunkMode = newMode;

    programContext.mapSection->SetCheckTileBeforePainting(newMode == program::EditorChunkMode::FixedChunks);
}