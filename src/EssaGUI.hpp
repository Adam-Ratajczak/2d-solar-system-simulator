#pragma once

#include "EssaCreateObject.hpp"
#include "FocusedObjectGUI.hpp"
#include "Object.hpp"
#include "SimulationView.hpp"
#include "World.hpp"
#include "gui/Button.hpp"
#include "gui/ColorPicker.hpp"
#include "gui/Container.hpp"
#include "gui/SettingsMenu.hpp"
#include "gui/Slider.hpp"
#include "gui/StateTextButton.hpp"
#include "gui/TextButton.hpp"
#include "gui/Textbox.hpp"
#include "gui/Textfield.hpp"
#include "gui/ValueSlider.hpp"
#include "gui/Widget.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <random>
#include <vector>

inline sf::Image load_image(std::string path) {
    // TODO: Error handling
    sf::Image image;
    image.loadFromFile(path);
    return image;
}

sf::Image load_image(std::string path);

class EssaGUI : public GUI::Container {
public:
    explicit EssaGUI(GUI::Application& application, World& world);

    virtual void handle_event(GUI::Event&) override;
    virtual void draw(sf::RenderWindow& window) const override;

    SimulationView& simulation_view() const { return *m_simulation_view; }

private:
    World& m_world;
    virtual void relayout() override;
    virtual void update() override;

    std::shared_ptr<GUI::ImageButton> m_toggle_camera_mode_at_focus;

    std::shared_ptr<GUI::Button> m_home_button;
    std::shared_ptr<GUI::Button> m_canvas_mode_button;

    std::shared_ptr<FocusedObjectGUI> m_focused_object_info;

    std::shared_ptr<SimulationView> m_simulation_view;

    std::shared_ptr<EssaCreateObject> m_create_object_gui;

    std::vector<std::shared_ptr<GUI::Textfield>> m_info_content;

    int m_saved_speed = 0;

    void m_switch_info(bool state);

    void m_create_cavas_mode_gui(Container& parent);
};
