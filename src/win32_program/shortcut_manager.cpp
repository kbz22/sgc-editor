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
    if (shortcut.key == ShortcutKey::None && shortcut.modifier == ShortcutModifier::None) {
        return action::ActionType::Default;
    }

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

std::vector<win32_program::Shortcut> win32_program::ShortcutManager::GetShortcutsForAction(action::ActionType actionId) const
{
    std::vector<Shortcut> result;

    for (const auto& contextPair : m_shortcuts)
    {
        for (const auto& shortcutPair : contextPair.second)
        {
            if (shortcutPair.second == actionId)
            {
                result.push_back(shortcutPair.first);
            }
        }
    }

    return result;
}

win32_program::ShortcutModifier win32_program::ShortcutManager::GetShortcutModifierFromKeyState()
{
    uint32_t modifiers = static_cast<uint32_t>(ShortcutModifier::None);

    if(GetKeyState(VK_CONTROL) & 0x8000)
        modifiers |= static_cast<uint32_t>(ShortcutModifier::Ctrl);

    if(GetKeyState(VK_SHIFT) & 0x8000)
        modifiers |= static_cast<uint32_t>(ShortcutModifier::Shift);

    if(GetKeyState(VK_MENU) & 0x8000)
        modifiers |= static_cast<uint32_t>(ShortcutModifier::Alt);

    return static_cast<ShortcutModifier>(modifiers);
}

std::vector<locale::StringId> win32_program::ShortcutManager::GetStringIdsForShortcutModifier(ShortcutModifier modifier)
{
    using namespace locale;
    std::vector<StringId> result;

    if(static_cast<uint32_t>(modifier) & static_cast<uint32_t>(ShortcutModifier::Ctrl))
        result.push_back(StringId::ShortcutNameCtrl);

    if(static_cast<uint32_t>(modifier) & static_cast<uint32_t>(ShortcutModifier::Alt))
        result.push_back(StringId::ShortcutNameAlt);

    if(static_cast<uint32_t>(modifier) & static_cast<uint32_t>(ShortcutModifier::Shift))
        result.push_back(StringId::ShortcutNameShift);

    return result;
}

std::optional<locale::StringId> win32_program::ShortcutManager::GetStringIdForShortcutKey(uint32_t key)
{
    using namespace locale;

    switch (static_cast<ShortcutKey>(key))
    {
        case ShortcutKey::Ctrl: return StringId::ShortcutNameCtrl;
        case ShortcutKey::Alt: return StringId::ShortcutNameAlt;
        case ShortcutKey::Shift: return StringId::ShortcutNameShift;
        case ShortcutKey::Insert: return StringId::ShortcutNameInsert;
        case ShortcutKey::Delete: return StringId::ShortcutNameDelete;
        case ShortcutKey::PageUp: return StringId::ShortcutNamePageUp;
        case ShortcutKey::PageDown: return StringId::ShortcutNamePageDown;
        case ShortcutKey::Home: return StringId::ShortcutNameHome;
        case ShortcutKey::End: return StringId::ShortcutNameEnd;
        case ShortcutKey::ArrowUp: return StringId::ShortcutNameArrowUp;
        case ShortcutKey::ArrowDown: return StringId::ShortcutNameArrowDown;
        case ShortcutKey::ArrowLeft: return StringId::ShortcutNameArrowLeft;
        case ShortcutKey::ArrowRight: return StringId::ShortcutNameArrowRight;
        default: return std::nullopt;
    }
}