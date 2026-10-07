
#include "program/program.hpp"

void program::ProgramContext::UpdateEditorLayerMode(program::EditorLayerMode newMode)
{
    auto mapDocument = m_fileManager->GetActiveDocument();

    if(mapDocument == nullptr) {
        return;
    }

    auto layerManager = mapDocument->GetLayerManager();

    m_editorLayerMode = newMode;

    m_actionManager->ActionSetChecked(action::ActionType::LayerModeMultilayer, newMode == program::EditorLayerMode::MultiLayer);
    m_actionManager->ActionSetChecked(action::ActionType::LayerModeSingleLayer, newMode == program::EditorLayerMode::SingleLayer);
    m_actionManager->ActionSetChecked(action::ActionType::LayerModeSingleImage, newMode == program::EditorLayerMode::SingleImage);
    
    switch(newMode) {

        case program::EditorLayerMode::SingleLayer:
        {
            layerManager->SetSingleLayerMode(true);                      
            break;
        }

        case program::EditorLayerMode::MultiLayer:
        {
            layerManager->SetSingleLayerMode(false);
            program::MultiLayerModeSetup(*this);
            break;
        }

        case program::EditorLayerMode::SingleImage:
        {
            layerManager->SetSingleLayerMode(false);
            program::SingleImageModeSetup(*this);
            break;
        }
    }

    m_mapSection->Refresh(*this);
    m_mapSection->Update();

    m_toolbarSection->Refresh(*this);
    m_menuSection->Refresh(*this);
}

void program::ProgramContext::UpdateEditorChunkMode(program::EditorChunkMode newMode)
{
    m_editorChunkMode = newMode;

    m_actionManager->ActionSetChecked(action::ActionType::ChunkModeFixedSize, newMode == program::EditorChunkMode::FixedChunks);
    m_actionManager->ActionSetChecked(action::ActionType::ChunkModeFree, newMode == program::EditorChunkMode::DynamicChunks);

    m_mapSection->SetCheckTileBeforePainting(newMode == program::EditorChunkMode::FixedChunks);

    m_toolbarSection->Refresh(*this);
    m_menuSection->Refresh(*this);
}

void program::ProgramContext::UpdateBrushMode(editor_tools::PaintMode newMode)
{
    m_mapSection->SetPaintMode(newMode);

    m_actionManager->ActionSetChecked(action::ActionType::PaintModeBrush, newMode == editor_tools::PaintMode::Brush);
    m_actionManager->ActionSetChecked(action::ActionType::PaintModeRectangle, newMode == editor_tools::PaintMode::Rectangle);
    m_actionManager->ActionSetChecked(action::ActionType::PaintModeFill, newMode == editor_tools::PaintMode::Fill);
    m_actionManager->ActionSetChecked(action::ActionType::PaintModeSelect, newMode == editor_tools::PaintMode::Select);
    m_actionManager->ActionSetChecked(action::ActionType::ChunkRemoverTool, newMode == editor_tools::PaintMode::ChunkRemover);
    m_actionManager->ActionSetChecked(action::ActionType::TilePickerTool, newMode == editor_tools::PaintMode::TilePicker);    

    m_toolbarSection->Refresh(*this);
    m_menuSection->Refresh(*this);
}

void program::ProgramContext::UpdateEditorGridMode(program::EditorGridMode newMode)
{
    m_editorGridMode = static_cast<program::EditorGridMode>(
        static_cast<uint8_t>(m_editorGridMode) ^
        static_cast<uint8_t>(newMode)
    );

    m_actionManager->ActionSetChecked(action::ActionType::GridModeTile, HasFlag(m_editorGridMode, program::EditorGridMode::TileGrid));
    m_actionManager->ActionSetChecked(action::ActionType::GridModeChunk, HasFlag(m_editorGridMode, program::EditorGridMode::ChunkGrid));

    m_toolbarSection->Refresh(*this);
    m_menuSection->Refresh(*this);
    m_mapSection->Refresh(*this);
    m_mapSection->Update();
}

void program::ProgramContext::UpdateEditorSelectionMode(editor_tools::SelectionMode newMode)
{
    bool selectionActive = m_mapSection->GetPaintMode() == editor_tools::PaintMode::Select;
    constexpr int selectionModeCount = 3;
    editor_tools::SelectionMode selectionOptions[selectionModeCount] = {
        editor_tools::SelectionMode::SingleLayer,
        editor_tools::SelectionMode::AllLayers,
        editor_tools::SelectionMode::VisibleLayers
    };
    action::ActionType selectionActionTypes[selectionModeCount] = {
        action::ActionType::SelectSingleLayerMode,
        action::ActionType::SelectAllLayersMode,
        action::ActionType::SelectVisibleLayersMode
    };

    if(selectionActive) 
    {
        for(int i=0; i<selectionModeCount; ++i) 
        {
            m_actionManager->ActionSetChecked(
                selectionActionTypes[i],
                newMode == selectionOptions[i]
            );
            m_actionManager->ActionSetEnabled(
                selectionActionTypes[i],
                true
            );
        }
        
        m_mapSection->SetSelectionMode(newMode);
    }
    else 
    {
        for(int i=0; i<selectionModeCount; ++i) 
        {
            m_actionManager->ActionSetChecked(
                selectionActionTypes[i],
                false
            );
            m_actionManager->ActionSetEnabled(
                selectionActionTypes[i],
                false
            );
        }
    }

    m_toolbarSection->Refresh(*this);
    m_menuSection->Refresh(*this);
}

void program::ProgramContext::UpdateEditorSelectionTools()
{
    constexpr int selectionToolsCount = 5;
    constexpr action::ActionType selectionToolsTypes[selectionToolsCount] = {
        action::ActionType::SelectionClear,
        action::ActionType::SelectionCut,
        action::ActionType::SelectionCopy,
        action::ActionType::SelectionPaste
    };

    auto selectionActive = m_mapSection->GetPaintMode() == editor_tools::PaintMode::Select;

    if(selectionActive) 
    {
        for(int i=0; i<selectionToolsCount; ++i) 
        {            
            m_actionManager->ActionSetChecked(
                selectionToolsTypes[i],
                false
            );
            m_actionManager->ActionSetEnabled(
                selectionToolsTypes[i],
                true
            );
        }

        m_actionManager->ActionSetChecked(
            action::ActionType::SelectionMove,
            m_mapSection->GetSelectionMoveMode()            
        );
        m_actionManager->ActionSetEnabled(
            action::ActionType::SelectionMove,
            true
        );
    }
    else 
    {
        for(int i=0; i<selectionToolsCount; ++i) 
        {
            m_actionManager->ActionSetChecked(
                selectionToolsTypes[i],
                false
            );
            m_actionManager->ActionSetEnabled(
                selectionToolsTypes[i],
                false
            );
        }

        m_actionManager->ActionSetChecked(
            action::ActionType::SelectionMove,
            false
        );
        m_actionManager->ActionSetEnabled(
            action::ActionType::SelectionMove,
            false
        );
        m_mapSection->SetSelectionMoveMode(false);
    }

    m_toolbarSection->Refresh(*this);
    m_menuSection->Refresh(*this);    
}