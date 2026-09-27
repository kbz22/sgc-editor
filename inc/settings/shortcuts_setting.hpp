#pragma once

#include "settings/isetting.hpp"
#include "settings/shortcut_settings_manager.hpp"
#include "win32_program/shortcut_manager.hpp"

#include <memory>

namespace settings 
{
    class ShortcutsSetting : public ISetting    
    {
        private:
            std::unique_ptr<ShortcutSettingsManager> m_shortcutSettingsManager;
            win32_program::ShortcutManager& m_shortcutManager;

        public:
            ShortcutsSetting(win32_program::ShortcutManager& shortcutManager);

            // getters
            ShortcutSettingsManager& GetShortcutSettingsManager() const;
            win32_program::ShortcutManager& GetShortcutManager() const;

            // Update
            void Commit() override;
            Key GetKey() override;
    };

}