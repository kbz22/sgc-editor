#pragma once

#include "win32_program/pointer_lock.hpp"

namespace program
{
    using win32_program::PointerMapKey, win32_program::PointerBehaviour, win32_program::Pointer, win32_program::PointerType;

    class PointerManager
    {
        private:
            std::unordered_map<PointerMapKey, PointerBehaviour> m_pointerBehaviour;

        public:
            void SetPointerBehaviour(PointerMapKey pointerKey, PointerBehaviour behaviour);

            bool CheckPointerBehaviour(PointerMapKey pointerKey, PointerBehaviour behaviour);
            bool CheckPointerBehaviour(Pointer pointer, PointerBehaviour behaviour);
    };
}