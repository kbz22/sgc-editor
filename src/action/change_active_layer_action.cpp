#include "action/change_active_layer_action.hpp"
#include "program/program.hpp"

action::ChangeActiveLayerAction::ChangeActiveLayerAction(ChangeActiveLayerDirection direction) :
    m_direction(direction)
{
    m_enabled = true;
    m_checked = false;

    switch(m_direction) 
    {
        case ChangeActiveLayerDirection::Up:
            m_actionType = ActionType::ChangeActiveLayerUp;            
            m_actionDescription.nameStringId = locale::StringId::NameChangeLayerUp;
            m_actionDescription.shortcuts = {
                { win32_program::ShortcutModifier::None, win32_program::ShortcutKey::PageUp }
            };
            break;

        case ChangeActiveLayerDirection::Down:
            m_actionType = ActionType::ChangeActiveLayerDown;            
            m_actionDescription.nameStringId = locale::StringId::NameChangeLayerDown;
            m_actionDescription.shortcuts = {
                { win32_program::ShortcutModifier::None, win32_program::ShortcutKey::PageDown }
            };
            break;

        case ChangeActiveLayerDirection::Top:
            m_actionType = ActionType::ChangeActiveLayerTop;            
            m_actionDescription.nameStringId = locale::StringId::NameChangeLayerTop;
            m_actionDescription.shortcuts = {
                { win32_program::ShortcutModifier::None, win32_program::ShortcutKey::Home }
            };
            break;

        case ChangeActiveLayerDirection::Bottom:
            m_actionType = ActionType::ChangeActiveLayerBottom;            
            m_actionDescription.nameStringId = locale::StringId::NameChangeLayerBottom;
            m_actionDescription.shortcuts = {
                { win32_program::ShortcutModifier::None, win32_program::ShortcutKey::End }
            };
            break;        
    }

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = -1;
    m_actionDescription.tooltipStringId = std::nullopt;
    m_actionDescription.checkGroupItem = false;    
    m_actionDescription.groupId = GroupId::LayerManagement;
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::MapEditor;
}

void action::ChangeActiveLayerAction::Execute(program::ProgramContext& context)
{
    auto mapDocument = context.GetManager<file::FileManager>()->GetActiveDocument();
    if(mapDocument != nullptr) 
    {
        auto layerManager = mapDocument->GetLayerManager();
        auto currentActiveLayerIndex = layerManager->GetActiveLayerIndex();
        auto layerCount = layerManager->GetSize();

        switch(m_direction) 
        {
            case ChangeActiveLayerDirection::Up:
            {
                if(currentActiveLayerIndex > 0)
                    layerManager->SetActiveLayerIndex(currentActiveLayerIndex - 1);
                break;
            }

            case ChangeActiveLayerDirection::Down:
            {
                if(currentActiveLayerIndex < layerCount - 1)
                    layerManager->SetActiveLayerIndex(currentActiveLayerIndex + 1);                
                break;
            }

            case ChangeActiveLayerDirection::Top:
            {
                layerManager->SetActiveLayerIndex(0);
                break;
            }

            case ChangeActiveLayerDirection::Bottom:
            {
                layerManager->SetActiveLayerIndex(layerManager->GetLayers().size() - 1);
                break;
            }
        }

        auto mapSection = context.GetSection<sections::MapSection>();
        auto layersSection = context.GetSection<sections::LayersSection>();
        mapSection->Update();
        layersSection->Refresh(context);
        layersSection->Update();
    }
}