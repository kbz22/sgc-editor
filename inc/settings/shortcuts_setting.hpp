#pragma once

#include "settings/isetting.hpp"
#include "program/shortcut_settings_manager.hpp"

namespace settings {

    class ShortcutsSetting : public ISetting    
    {
        private:
            program::ShortcutSettingsManager& m_shortcutSettingsManager;

        public:
            ShortcutsSetting(program::ShortcutSettingsManager& shortcutSettingsManager);

            // Serialization
            std::vector<uint8_t> GetBytes() const override = 0;
            void LoadFromBytes(const std::vector<uint8_t>& bytes) override = 0;

            // Update
            void Commit() override = 0;
    };

}