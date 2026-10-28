#include "settings/pointer_behaviour_setting.hpp"

settings::PointerBehaviourSetting::PointerBehaviourSetting(program::PointerManager &pointerManager) :
    m_pointerManager{pointerManager}
{}

void settings::PointerBehaviourSetting::SetBehaviour(win32_program::PointerMapKey pointerKey, win32_program::PointerBehaviour behaviour)
{
    m_behaviour[pointerKey] = behaviour;
}

void settings::PointerBehaviourSetting::SetBehaviours(PointerBehaviourMap map)
{
    m_behaviour = map;
}

void settings::PointerBehaviourSetting::Commit()
{
    for(auto &[pointerKey, pointerBehaviour] : m_behaviour)
    {
        if(m_behaviour.contains(pointerKey)){
            m_pointerManager.SetPointerBehaviour(pointerKey, pointerBehaviour);
        }
        else {
            m_pointerManager.SetPointerBehaviour(pointerKey, win32_program::PointerBehaviour::None);
        }
    }
}

settings::Key settings::PointerBehaviourSetting::GetKey() const
{
    return settings::Key::PointerBehaviourSetting;
}

win32_program::PointerBehaviour settings::PointerBehaviourSetting::GetBehaviour(win32_program::PointerMapKey pointerKey)
{
    if(!m_behaviour.contains(pointerKey))
    {
        return win32_program::PointerBehaviour::None;
    }
    
    return m_behaviour[pointerKey];
}

settings::PointerBehaviourMap settings::PointerBehaviourSetting::GetBehaviours() const
{
    return m_behaviour;
}