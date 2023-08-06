#pragma once

#include "../Object.hpp"
#include "../SimulationView.hpp"
#include "../World.hpp"
#include "EssaCreateObject.hpp"
#include "EssaSettings.hpp"
#include "FocusedObjectGUI.hpp"

#include <Essa/GUI/Overlays/MessageBox.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Widgets/Button.hpp>
#include <Essa/GUI/Widgets/ColorPicker.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/NotificationContainer.hpp>
#include <Essa/GUI/Widgets/SettingsMenu.hpp>
#include <Essa/GUI/Widgets/Slider.hpp>
#include <Essa/GUI/Widgets/StateTextButton.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/Textbox.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>
#include <Essa/GUI/Widgets/ValueSlider.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>

#include <memory>
#include <random>
#include <vector>

class EssaGUI : public GUI::Container {
public:
    explicit EssaGUI(World& world);

    virtual void on_init() override;

    virtual void draw(Gfx::Painter& window) const override;

    GUI::MDI::Host& mdi_host() { return *m_mdi_host; }
    SimulationView& simulation_view() const { return *m_simulation_view; }
    EssaSettings& settings_gui() { return *m_settings_gui; }
    GUI::NotificationContainer& notification_window() const { return *m_notification_container; }

private:
    World& m_world;
    virtual void update() override;
    virtual EventHandlerResult on_key_press(GUI::Event::KeyPress const&) override;

    GUI::MDI::Host* m_mdi_host = nullptr;
    SimulationView* m_simulation_view = nullptr;
    EssaCreateObject* m_create_object_gui = nullptr;
    EssaSettings* m_settings_gui = nullptr;
    GUI::NotificationContainer* m_notification_container = nullptr;

    bool m_draw_forward_simulation = false;

    void m_switch_info(bool state);
    void open_python_repl();
};
