#include "action/selection_clear_action.hpp"
#include "program/program.hpp"
#include "command/paint_selection_command.hpp"

action::SelectionClearAction::SelectionClearAction() 
{
    m_actionType = ActionType::SelectionClear;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 23;
    m_actionDescription.toolbarOrder = 2000;
    m_actionDescription.menuOrder = 600;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::SelectionTools;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorSelectionClear;
    m_actionDescription.nameStringId = locale::StringId::NameEditorSelectionClear;
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::MapEditorSelection;
    m_actionDescription.shortcuts = {
        {win32_program::ShortcutModifier::None, win32_program::ShortcutKey::Delete}
    };
}

void action::SelectionClearAction::Execute(program::ProgramContext& context)
{
    auto mapSection = context.GetSection<sections::MapSection>();
    auto selectionMode = mapSection->GetSelectionMode();
    auto mapDocument = context.GetManager<file::FileManager>()->GetActiveDocument();

    if(mapDocument == nullptr) {
        return;
    }

    auto layerManager = mapDocument->GetLayerManager();
    auto commandManager = mapDocument->GetCommandManager();
    auto selectionRectPos = mapSection->GetSelectionRectanglePositionTiles();
    auto selectionRectSize = mapSection->GetSelectionRectangleSizeTiles();

    command::MultilayerTileChangesType tileChanges;

    auto iterateTiles = [layerManager, &tileChanges, &selectionRectPos, &selectionRectSize](sgc::data::ITileStorage *layer, size_t layerIndex, sgc::tile::TileId clearTileId)
    {
        for(auto y = selectionRectPos.y; y < selectionRectPos.y + selectionRectSize.y; ++y) {
            for(auto x = selectionRectPos.x; x < selectionRectPos.x + selectionRectSize.x; ++x) {
                auto tilePos = sgc::tile::TilePosition2D{x,y};
                auto oldTileIdOpt = layer->GetTileAt(tilePos);

                if(!oldTileIdOpt.has_value()) {
                    continue;
                }

                if(oldTileIdOpt.value() != clearTileId) {
                    tileChanges[layerIndex][tilePos] = {
                        layerIndex,
                        tilePos,
                        oldTileIdOpt.value(),
                        clearTileId
                    };
                }
            }
        }
    };

    switch(selectionMode) {
        case editor_tools::SelectionMode::SingleLayer:
        {
            auto activeLayerIndex = layerManager->GetActiveLayerIndex();
            auto activeLayer = layerManager->GetLayers()[activeLayerIndex].storage;

            if(activeLayer != nullptr) {
                iterateTiles(
                    activeLayer.get(),
                    activeLayerIndex,
                    context.GetSection<sections::TilesetSection>()->GetClearTileId()
                );

                commandManager->Execute(std::make_unique<command::PaintSelectionCommand>(
                    *mapDocument,
                    tileChanges
                ));
            }

            break;
        }

        case editor_tools::SelectionMode::AllLayers:
        {
            auto layers = layerManager->GetLayers();

            for(size_t layerIndex = 0; layerIndex < layers.size(); ++layerIndex) {
                auto layer = layers[layerIndex].storage;

                if(layer != nullptr) {
                    iterateTiles(
                        layer.get(),
                        layerIndex,
                        context.GetSection<sections::TilesetSection>()->GetClearTileId()
                    );
                }                
            }

            commandManager->Execute(std::make_unique<command::PaintSelectionCommand>(
                *mapDocument,
                tileChanges
            ));

            break;
        }

        case editor_tools::SelectionMode::VisibleLayers:
        {
            auto layers = layerManager->GetLayers();

            for(size_t layerIndex = 0; layerIndex < layers.size(); ++layerIndex) {
                auto layer = layers[layerIndex].storage;

                if(layer != nullptr && layers[layerIndex].visible) {
                    iterateTiles(
                        layer.get(),
                        layerIndex,
                        context.GetSection<sections::TilesetSection>()->GetClearTileId()
                    );
                }
            }

            commandManager->Execute(std::make_unique<command::PaintSelectionCommand>(
                *mapDocument,
                tileChanges
            ));
            
            break;
        }

    }

    mapSection->Update();
}