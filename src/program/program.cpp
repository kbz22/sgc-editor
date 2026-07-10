#include "win32_program/windows_init.hpp"
#undef CreateWindow // avoid macro name conflict with sdl::CreateWindow
#include "win32_program/windows_controls.hpp"
#include "program/program.hpp"
#include "program/layer_manager.hpp"
#include <sgc/data/chunkedtilestorage.hpp>

program::ProgramContext& program::GetProgramContext()
{
    static ProgramContext context = {};
    return context;
}

void program::StartEditor(std::wstring tilesetPath, int tileWidth, int tileHeight, int chunksSizeX, int chunksSizeY)
{
    (void)chunksSizeX;
    (void)chunksSizeY;
    
    auto& programContext = GetProgramContext();    

    if(programContext.selectionRectangleOnTileset != nullptr) {
        programContext.selectionRectangleOnTileset.reset();
    }

    programContext.selectionRectangleOnTileset = std::make_unique<sgc::graphics::Rectangle>(
        sgc::math::vec2{ 0, 0 },
        sgc::math::uvec2{ static_cast<sgc::math::uval>(tileWidth), static_cast<sgc::math::uval>(tileHeight) }
    );
    programContext.selectionRectangleOnTileset->SetColor({ 0, 128, 255, 128 });

    programContext.tilesetSection->LoadTileset(tilesetPath);

    programContext.mapSection->LoadTileset(tilesetPath);

    programContext.mapSection->HandleSectionResize();
    programContext.tilesetSection->HandleSectionResize();
    programContext.mapSection->Update();
    programContext.tilesetSection->Update();

    programContext.layerManager = std::make_unique<program::LayerManager>();

    programContext.layerManager->AddLayer({
        std::make_shared<sgc::data::ChunkedTileStorage>(),
        L"Layer 0"
    });
    programContext.layerManager->SetActiveLayerIndex(0);
    programContext.layerManager->SetBaseLayerIndex(0);
    programContext.layerManager->GetLayers()[0]
        .storage->SetTileAt({ 0, 0 }, 1);

    programContext.mapSection->Refresh();
    programContext.mapSection->Update();
}

void program::HandleResize()
{
    /* auto& programContext = GetProgramContext();

    if (programContext.tilesetSection != nullptr) {
        programContext.tilesetSection->HandleSectionResize();        
    }

    if (programContext.mapSection != nullptr) {
        programContext.mapSection->HandleSectionResize();        
    } */
}