#pragma once

#include "Container.hpp"
#include "ToggleButton.hpp"
#include <vector>

class SettingsMenu : public Container {
public:
    explicit SettingsMenu(Container& c);

    struct MenuEntry {
        ToggleButton* button {};
        Container* settings_container {};
        std::function<void(bool)> on_toggle;
    };

    MenuEntry& add_entry(sf::Image const& image, std::string tooltip);

private:
    std::vector<MenuEntry> m_entries;
    Container* m_buttons_container {};
    Container* m_settings_container {};
};
