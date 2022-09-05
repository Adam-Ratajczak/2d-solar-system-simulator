#pragma once

#include <EssaGUI/gui/MessageBox.hpp>

#include "../Object.hpp"
#include "../SimulationView.hpp"
#include "../World.hpp"
#include "EssaCreateObject.hpp"
#include "EssaSettings.hpp"
#include "FocusedObjectGUI.hpp"
#include <EssaGUI/gui/Button.hpp>
#include <EssaGUI/gui/ColorPicker.hpp>
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/SettingsMenu.hpp>
#include <EssaGUI/gui/Slider.hpp>
#include <EssaGUI/gui/StateTextButton.hpp>
#include <EssaGUI/gui/TextButton.hpp>
#include <EssaGUI/gui/Textbox.hpp>
#include <EssaGUI/gui/Textfield.hpp>
#include <EssaGUI/gui/ToolWindow.hpp>
#include <EssaGUI/gui/ValueSlider.hpp>
#include <EssaGUI/gui/Widget.hpp>

#include <memory>
#include <random>
#include <vector>

class EssaGUI : public GUI::Container {
public:
    explicit EssaGUI(World& world);

    virtual void on_add() override;

    virtual void handle_event(GUI::Event&) override;
    virtual void draw(GUI::Window& window) const override;

    SimulationView& simulation_view() const { return *m_simulation_view; }

    EssaSettings& settings_gui() { return *m_settings_gui; }

private:
    World& m_world;
    virtual void update() override;

    SimulationView* m_simulation_view = nullptr;
    EssaCreateObject* m_create_object_gui = nullptr;
    EssaSettings* m_settings_gui = nullptr;

    bool m_draw_forward_simulation = false;

    void m_switch_info(bool state);
    void open_python_repl();
};
