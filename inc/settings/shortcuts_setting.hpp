#pragma once

#include "settings/isetting.hpp"
#include "settings/shortcut_settings_manager.hpp"
#include "win32_program/shortcut_manager.hpp"

#include <memory>

namespace settings {

    constexpr unsigned ShortcutSettingKey = 1;

    class ShortcutsSetting : public ISetting    
    {
        private:
            std::unique_ptr<ShortcutSettingsManager> m_shortcutSettingsManager;
            win32_program::ShortcutManager& m_shortcutManager;

        public:
            ShortcutsSetting(win32_program::ShortcutManager& shortcutManager);

            // Serialization
            std::vector<uint8_t> GetBytes() const override;
            void LoadFromBytes(const std::vector<uint8_t>& bytes) override;

            // getters
            ShortcutSettingsManager& GetShortcutSettingsManager() const;
            win32_program::ShortcutManager& GetShortcutManager() const;

            // Update
            void Commit() override;
    };

}