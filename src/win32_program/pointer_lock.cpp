#include "win32_program/pointer_lock.hpp"

bool win32_program::PointerLock::IsLocked() const {
    return m_isLocked;
}

bool win32_program::PointerLock::IsLockedBy(Pointer pointer) const {
    return m_isLocked && m_pointer == pointer;
}

bool win32_program::PointerLock::Acquire(Pointer pointer) {
    if(m_isLocked) {
        return false;
    }

    m_pointer = pointer;
    m_isLocked = true;
    return true;
}

bool win32_program::PointerLock::Release(Pointer pointer) {
    if(IsLockedBy(pointer)) {
        m_isLocked = false;
        return true;
    }
    return false;
}

win32_program::PointerType win32_program::PointerLock::GetPointerType() const
{
    return m_pointer.Type;
}

uint32_t win32_program::PointerLock::GetPointerId() const
{
    return m_pointer.Id;
}

