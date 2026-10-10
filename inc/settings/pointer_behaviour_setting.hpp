#pragma once

#include "settings/isetting.hpp"
#include "sections/map_section.hpp"
#include <unordered_map>

namespace settings
{
    using PointerBehaviourMap = std::unordered_map<sections::PointerType, sections::PointerBehaviour>;

    class PointerBehaviourSetting : public ISetting
    {
        private:
            sections::MapSection &m_mapSection;
            PointerBehaviourMap m_behaviour{
                {sections::PointerType::LeftMouse, sections::PointerBehaviour::Painting},                
                {sections::PointerType::MiddleMouse, sections::PointerBehaviour::Panning},
                {sections::PointerType::Touch, sections::PointerBehaviour::Panning},
                {sections::PointerType::Pen, sections::PointerBehaviour::Painting},
            };

        public:
            PointerBehaviourSetting(sections::MapSection &mapSection);

            void SetBehaviour(sections::PointerType pointerType, sections::PointerBehaviour behaviour);
            void SetBehaviours(PointerBehaviourMap map);
            sections::PointerBehaviour GetBehaviour(sections::PointerType type);
            PointerBehaviourMap GetBehaviours() const;

            void Commit() override;
            Key GetKey() const override;
    };
}