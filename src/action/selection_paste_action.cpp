#include "action/selection_paste_action.hpp"
#include "win32_program/clipboard.hpp"
#include "win32_program/map_selection_serializer.hpp"
#include "program/program.hpp"
#include "command/paint_selection_command.hpp"

action::SelectionPasteAction::SelectionPasteAction()
{
    m_actionType = ActionType::SelectionPaste;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 27;
    m_actionDescription.toolbarOrder = 2400;
    m_actionDescription.menuOrder = 500;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::SelectionTools;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorSelectionPaste;
    m_actionDescription.nameStringId = locale::StringId::NameEditorSelectionPaste;
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::MapEditorSelection;
    m_actionDescription.shortcuts = {
        {win32_program::ShortcutModifier::Ctrl, 'V'},
        {win32_program::ShortcutModifier::Shift, win32_program::ShortcutKey::Insert}
    };
}

void action::SelectionPasteAction::Execute(program::ProgramContext &context)
{
    using namespace win32_program;

    auto mapDocument = context.fileManager->GetActiveDocument();

    if(mapDocument == nullptr)
    {
        return;
    }

    auto layerManager = mapDocument->GetLayerManager();
    auto commandManager = mapDocument->GetCommandManager();
    auto selectionMode = context.mapSection->GetSelectionMode();
    auto currentSelectionPosition = context.mapSection->GetSelectionRectanglePositionTiles();
    MapSelection copiedSelection;
    Clipboard::Get<MapSelection>(copiedSelection);   

    command::MultilayerTileChangesType tileChanges;
    auto cursorPosition = context.mapSection->GetCursorPositionInTiles();

    auto iterateTiles = [layerManager, &copiedSelection, &tileChanges, &currentSelectionPosition](sgc::data::ITileStorage *layer, size_t pastedLayerIndex, size_t copiedLayerIndex)
    {
        auto &tiles = copiedSelection.layerTiles[copiedLayerIndex];
        auto tileCount = tiles.size();        

        for(int i=0;i<tileCount;i++)
        {
            auto x = i % copiedSelection.size.x;
            auto y = i / copiedSelection.size.x;
            auto tilePos = sgc::tile::TilePosition2D{x,y} + currentSelectionPosition;

            tileChanges[pastedLayerIndex][tilePos] = {
                pastedLayerIndex,
                tilePos,
                layer->GetTileAt(tilePos),
                tiles[i]
            };
        }
    };

    switch(selectionMode)
    {
        case editor_tools::SelectionMode::SingleLayer:
        {   
            auto activeLayerIndex = layerManager->GetActiveLayerIndex();         
            auto activeLayer = layerManager->GetLayers()[activeLayerIndex].storage;
            auto copiedLayerIndex = activeLayerIndex;

            if(!copiedSelection.layerTiles.contains(activeLayerIndex))
            {
                auto beginIterator = copiedSelection.layerTiles.begin();
                if(beginIterator != copiedSelection.layerTiles.end()){
                    copiedLayerIndex = beginIterator->first;
                }
            }

            if(activeLayer != nullptr) {
                iterateTiles(
                    activeLayer.get(),
                    activeLayerIndex,
                    copiedLayerIndex
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

                if(layer != nullptr)
                {
                    iterateTiles(
                        layer.get(),
                        layerIndex,
                        layerIndex
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
                auto layer = layers[layerIndex];

                if(layer.storage != nullptr && layer.visible)
                {
                    iterateTiles(
                        layer.storage.get(),
                        layerIndex,
                        layerIndex
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

    context.mapSection->Update();

    return;
}