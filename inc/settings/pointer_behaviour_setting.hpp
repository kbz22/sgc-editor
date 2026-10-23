#pragma once

#include "settings/isetting.hpp"
#include "sections/map_section.hpp"
#include <unordered_map>

namespace settings
{    
    using PointerBehaviourMap = std::unordered_map<sections::PointerMapKey, sections::PointerBehaviour>;

    class PointerBehaviourSetting : public ISetting
    {
        private:
            sections::MapSection &m_mapSection;
            PointerBehaviourMap m_behaviour{
                // {sections::PointerType::LeftMouse, sections::PointerBehaviour::Painting},
                // {sections::PointerType::MiddleMouse, sections::PointerBehaviour::Panning},
                {{sections::PointerType::Mouse, 0}, sections::PointerBehaviour::Painting},
                {{sections::PointerType::Mouse, 2}, sections::PointerBehaviour::Panning},
                {{sections::PointerType::Touch, 0}, sections::PointerBehaviour::Panning},
                {{sections::PointerType::Pen, 0}, sections::PointerBehaviour::Painting},
            };

        public:
            PointerBehaviourSetting(sections::MapSection &mapSection);

            void SetBehaviour(sections::PointerMapKey pointerKey, sections::PointerBehaviour behaviour);
            void SetBehaviours(PointerBehaviourMap map);
            sections::PointerBehaviour GetBehaviour(sections::PointerMapKey pointerKey);
            PointerBehaviourMap GetBehaviours() const;

            void Commit() override;
            Key GetKey() const override;
    };
}