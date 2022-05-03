#pragma once

#include "../Object.hpp"
#include "../SimulationView.hpp"
#include "../World.hpp"
#include "../gui/Button.hpp"
#include "../gui/ColorPicker.hpp"
#include "../gui/Container.hpp"
#include "../gui/SettingsMenu.hpp"
#include "../gui/Slider.hpp"
#include "../gui/StateTextButton.hpp"
#include "../gui/TextButton.hpp"
#include "../gui/Textbox.hpp"
#include "../gui/Textfield.hpp"
#include "../gui/ValueSlider.hpp"
#include "../gui/Widget.hpp"
#include "EssaCanvasMode.hpp"
#include "EssaCreateObject.hpp"
#include "EssaSettings.hpp"
#include "FocusedObjectGUI.hpp"

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
    explicit EssaGUI(GUI::WidgetTreeRoot&, World& world);

    virtual void handle_event(GUI::Event&) override;
    virtual void draw(sf::RenderWindow& window) const override;

    SimulationView& simulation_view() const { return *m_simulation_view; }

private:
    World& m_world;
    virtual void update() override;

    SimulationView* m_simulation_view = nullptr;
    std::list<FocusedObjectGUI*> m_focused_object_gui;
    EssaCreateObject* m_create_object_gui = nullptr;
    EssaSettings* m_settings_gui = nullptr;
    EssaCanvasMode* m_canvas_mode_gui = nullptr;

    int m_saved_speed = 0;
    bool m_draw_forward_simulation = false;

    void m_switch_info(bool state);
    void m_pause_simulation(bool state);
};
