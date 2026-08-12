#include "win32_program/shortcut_manager.hpp"

win32_program::ShortcutManager::ShortcutManager(HINSTANCE hInstance, HWND hwnd) :
    hInstance(hInstance),
    hwnd(hwnd)
{}

void win32_program::ShortcutManager::RegisterShortcut(
    const Shortcut& shortcut,
    ShortcutContext context,
    action::ActionType actionId
)
{
    m_shortcuts[context][shortcut] = actionId;
}

action::ActionType win32_program::ShortcutManager::GetActionForShortcut(
    const Shortcut& shortcut,
    ShortcutContext context
) const
{
    auto contextIt = m_shortcuts.find(context);
    if (contextIt == m_shortcuts.end()) {
        return action::ActionType::Default;
    }

    const auto& shortcutsMap = contextIt->second;
    auto shortcutIt = shortcutsMap.find(shortcut);
    if (shortcutIt == shortcutsMap.end()) {
        return action::ActionType::Default;
    }

    return shortcutIt->second;
}

win32_program::ShortcutModifier win32_program::ShortcutManager::GetShortcutModifierFromKeyState()
{
    ShortcutModifier modifiers = ShortcutModifier::None;

    if(GetKeyState(VK_CONTROL) & 0x8000)
        modifiers = static_cast<ShortcutModifier>(static_cast<uint32_t>(modifiers) | static_cast<uint32_t>(ShortcutModifier::Ctrl));

    if(GetKeyState(VK_SHIFT) & 0x8000)
        modifiers = static_cast<ShortcutModifier>(static_cast<uint32_t>(modifiers) | static_cast<uint32_t>(ShortcutModifier::Shift));

    if(GetKeyState(VK_MENU) & 0x8000)
        modifiers = static_cast<ShortcutModifier>(static_cast<uint32_t>(modifiers) | static_cast<uint32_t>(ShortcutModifier::Alt));

    return modifiers;
}

