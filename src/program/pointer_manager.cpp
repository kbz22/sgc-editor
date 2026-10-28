#include "program/pointer_manager.hpp"

void program::PointerManager::SetPointerBehaviour(PointerMapKey pointerKey, PointerBehaviour behaviour)
{    
    if(behaviour == PointerBehaviour::None)
    {
        m_pointerBehaviour.erase(pointerKey);
    }
    else
    {
        m_pointerBehaviour[pointerKey] = behaviour;
    }
}

bool program::PointerManager::CheckPointerBehaviour(win32_program::PointerMapKey pointerKey, win32_program::PointerBehaviour behaviour)
{
    return m_pointerBehaviour.contains(pointerKey) && m_pointerBehaviour[pointerKey] == behaviour;
}

bool program::PointerManager::CheckPointerBehaviour(win32_program::Pointer pointer, win32_program::PointerBehaviour behaviour)
{
    PointerMapKey pointerKey;
    pointerKey = pointer;
    return CheckPointerBehaviour(pointerKey, behaviour);
}