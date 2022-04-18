#pragma once

#include "Container.hpp"
#include "ImageButton.hpp"
#include <vector>

namespace GUI {

class SettingsMenu : public Container {
public:
    explicit SettingsMenu(Container& c);

    struct MenuEntry {
        ImageButton* button {};
        Container* settings_container {};
        std::function<void(bool)> on_toggle;
    };

    MenuEntry& add_entry(sf::Image const& image, std::string tooltip);

private:
    virtual bool isolated_focus() const override { return true; }

    std::vector<MenuEntry> m_entries;
    Container* m_buttons_container {};
    Container* m_settings_container {};
};

}
