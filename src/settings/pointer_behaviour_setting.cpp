#include "settings/pointer_behaviour_setting.hpp"

settings::PointerBehaviourSetting::PointerBehaviourSetting(sections::MapSection &mapSection) :
    m_mapSection{mapSection}
{}

void settings::PointerBehaviourSetting::SetBehaviour(sections::PointerType pointerType, sections::PointerBehaviour behaviour)
{
    m_behaviour[pointerType] = behaviour;
}

void settings::PointerBehaviourSetting::SetBehaviours(PointerBehaviourMap map)
{
    m_behaviour = map;
}

void settings::PointerBehaviourSetting::Commit()
{
    for(auto &[pointerType, pointerBehaviour] : m_behaviour)
    {
        if(m_behaviour.contains(pointerType)){
            m_mapSection.SetPointerBehaviour(pointerType, pointerBehaviour);
        }
        else {
            m_mapSection.SetPointerBehaviour(pointerType, sections::PointerBehaviour::None);
        }
    }
}

settings::Key settings::PointerBehaviourSetting::GetKey() const
{
    return settings::Key::PointerBehaviourSetting;
}

sections::PointerBehaviour settings::PointerBehaviourSetting::GetBehaviour(sections::PointerType type)
{
    if(!m_behaviour.contains(type))
    {
        return sections::PointerBehaviour::None;
    }
    
    return m_behaviour[type];
}

settings::PointerBehaviourMap settings::PointerBehaviourSetting::GetBehaviours() const
{
    return m_behaviour;
}