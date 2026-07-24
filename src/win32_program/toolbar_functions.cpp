#include "win32_program/toolbar_functions.hpp"

#include "sgc_view/sgc_view.hpp"
#include "win32_program/windows_init.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "program/program.hpp"
#include "program/except.hpp"
#include "defaults.hpp"

#include <fstream>
#include <vector>

#include "new_file_dialog.h"
#include <commdlg.h>

#include "command/layer_add_command.hpp"
#include "command/layer_remove_command.hpp"
#include "command/layer_move_command.hpp"

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