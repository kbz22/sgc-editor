#pragma once

#include "settings/isetting.hpp"
#include "win32_program/pointer_lock.hpp"
#include "sections/map_section.hpp"
#include <unordered_map>

namespace settings
{    
    using win32_program::PointerType, win32_program::PointerBehaviour, win32_program::PointerMapKey;
    using PointerBehaviourMap = std::unordered_map<PointerMapKey, PointerBehaviour>;    

    class PointerBehaviourSetting : public ISetting
    {
        private:
            sections::MapSection &m_mapSection;
            PointerBehaviourMap m_behaviour{
                {{PointerType::Mouse, 0}, PointerBehaviour::Painting},
                {{PointerType::Mouse, 2}, PointerBehaviour::Panning},
                {{PointerType::Touch, 0}, PointerBehaviour::Panning},
                {{PointerType::Pen, 0}, PointerBehaviour::Painting},
            };

        public:
            PointerBehaviourSetting(sections::MapSection &mapSection);

            void SetBehaviour(PointerMapKey pointerKey, PointerBehaviour behaviour);
            void SetBehaviours(PointerBehaviourMap map);
            PointerBehaviour GetBehaviour(PointerMapKey pointerKey);
            PointerBehaviourMap GetBehaviours() const;

            void Commit() override;
            Key GetKey() const override;
    };
}