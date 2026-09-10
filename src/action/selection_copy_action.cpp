#include "action/selection_copy_action.hpp"
#include "win32_program/map_selection_serializer.hpp"
#include "win32_program/clipboard.hpp"
#include "program/program.hpp"

action::SelectionCopyAction::SelectionCopyAction()
{
    m_actionType = ActionType::SelectionCopy;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 25;
    m_actionDescription.toolbarOrder = 2300;
    m_actionDescription.menuOrder = 400;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::SelectionTools;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorSelectionCopy;
    m_actionDescription.nameStringId = locale::StringId::NameEditorSelectionCopy;
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::MapEditorSelection;
    m_actionDescription.shortcuts = {
        {win32_program::ShortcutModifier::Ctrl, 'C'}
    };
}

void action::SelectionCopyAction::Execute(program::ProgramContext &context)
{
    using namespace win32_program;

    auto mapDocument = context.GetManager<file::FileManager>()->GetActiveDocument();

    if(mapDocument == nullptr)
    {
        return;
    }

    MapSelection selection{};
    auto mapSection = context.GetSection<sections::MapSection>();
    selection.startPoint = mapSection->GetSelectionRectanglePositionTiles();
    selection.size = mapSection->GetSelectionRectangleSizeTiles();

    auto layerManager = mapDocument->GetLayerManager();  
    auto clearTileId = context.GetSection<sections::TilesetSection>()->GetClearTileId();

    auto iterateTiles = [layerManager, &selection, clearTileId](sgc::data::ITileStorage *layer, size_t layerIndex)
    {
        selection.layerTiles[layerIndex] = std::vector<sgc::tile::TileId>{};
        for(auto y = selection.startPoint.y; y < selection.startPoint.y + selection.size.y; ++y) {
            for(auto x = selection.startPoint.x; x < selection.startPoint.x + selection.size.x; ++x)
            {
                sgc::tile::TileId tileId = clearTileId;
                auto opt = layer->GetTileAt({x,y});

                if(opt.has_value()) {
                    tileId = opt.value();
                }

                selection.layerTiles[layerIndex].push_back(tileId);
            }
        }
    };

    auto selectionMode = mapSection->GetSelectionMode();

    switch(selectionMode)
    {
        case editor_tools::SelectionMode::SingleLayer:
        {
            auto activeLayerIndex = layerManager->GetActiveLayerIndex();
            auto activeLayer = layerManager->GetLayers()[activeLayerIndex].storage;

            if(activeLayer != nullptr)
            {
                iterateTiles(activeLayer.get(), activeLayerIndex);
            }

            break;
        }

        case editor_tools::SelectionMode::AllLayers:
        {
            auto layers = layerManager->GetLayers();

            for(size_t layerIndex = 0; layerIndex < layers.size(); ++layerIndex) {
                auto layer = layers[layerIndex].storage;

                if(layer != nullptr) {
                    iterateTiles(layer.get(), layerIndex);
                }
            }

            break;
        }

        case editor_tools::SelectionMode::VisibleLayers:
        {
            auto layers = layerManager->GetLayers();

            for(size_t layerIndex = 0; layerIndex < layers.size(); ++layerIndex) {
                auto layer = layers[layerIndex].storage;

                if(layer != nullptr && layers[layerIndex].visible) {
                    iterateTiles(layer.get(), layerIndex);
                }
            }

            break;
        }
    }

    Clipboard::Set<MapSelection>(selection);
}