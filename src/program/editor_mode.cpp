#include "program/editor_mode.hpp"
#include "program/program.hpp"

void program::MultiLayerModeSetup(ProgramContext& context)
{
    auto &document = *context.fileManager->GetActiveDocument();
    auto manager = document.GetLayerManager();
    auto layerCount = manager->GetSize();
    auto activeLayerIndex = manager->GetActiveLayerIndex();

    for(size_t i = 0; i < layerCount; ++i) {
        if(i != activeLayerIndex) {
            manager->SetLayerTransparency(i, 128);
        } else {
            manager->SetLayerTransparency(i, 255);
        }
    }
}

void program::SingleImageModeSetup(ProgramContext& context)
{
    auto &document = *context.fileManager->GetActiveDocument();
    auto manager = document.GetLayerManager();
    auto layerCount = manager->GetSize();

    for(size_t i = 0; i < layerCount; ++i) {
        manager->SetLayerTransparency(i, 255);
    }
}