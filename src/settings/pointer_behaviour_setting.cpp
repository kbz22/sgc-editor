#include "settings/pointer_behaviour_setting.hpp"

settings::PointerBehaviourSetting::PointerBehaviourSetting(sections::MapSection &mapSection) :
    m_mapSection{mapSection}
{}

void settings::PointerBehaviourSetting::SetBehaviour(sections::PointerMapKey pointerKey, sections::PointerBehaviour behaviour)
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
            m_mapSection.SetPointerBehaviour(pointerKey, pointerBehaviour);
        }
        else {
            m_mapSection.SetPointerBehaviour(pointerKey, sections::PointerBehaviour::None);
        }
    }
}

settings::Key settings::PointerBehaviourSetting::GetKey() const
{
    return settings::Key::PointerBehaviourSetting;
}

sections::PointerBehaviour settings::PointerBehaviourSetting::GetBehaviour(sections::PointerMapKey pointerKey)
{
    if(!m_behaviour.contains(pointerKey))
    {
        return sections::PointerBehaviour::None;
    }
    
    return m_behaviour[pointerKey];
}

settings::PointerBehaviourMap settings::PointerBehaviourSetting::GetBehaviours() const
{
    return m_behaviour;
}