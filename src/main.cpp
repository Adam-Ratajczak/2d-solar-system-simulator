// keep first!
#include <GL/glew.h>

#include "World.hpp"
#include "essagui/EssaGUI.hpp"
#include "essagui/EssaSplash.hpp"
#include "pyssa/Environment.hpp"

#include <Essa/GUI/Application.hpp>
#include <iostream>

int main() {
    // TODO: set framerate limit

    World world;

    GUI::Application application;
    auto& host_window = application.create_host_window({ 1000, 1000 }, "ESSA");
    auto& gui = host_window.set_main_widget<EssaGUI>(world);

    world.reset({});

    // PySSA test
#ifdef ENABLE_PYSSA
    PySSA::Environment env(world, gui.simulation_view());
    if (!env.run_script("test.py")) {
        std::cout << "Failed to execute python script :(" << std::endl;
    }
#endif

    auto& notification_window = host_window.open_overlay<GUI::NotificationWindow>();
    notification_window.set_size({ 200, 500 });
    notification_window.set_position({ host_window.size().x() / 2 - notification_window.size().x() / 2, 10 });
    gui.set_notification_window(notification_window);

    host_window.on_event = [&](GUI::Event const& event) -> GUI::Widget::EventHandlerResult {
        if (auto* resize = event.get<GUI::Event::WindowResize>()) {
            notification_window.set_position({ resize->new_size().x() / 2 - notification_window.size().x() / 2, 10 });
        }
        return GUI::Widget::EventHandlerResult::NotAccepted;
    };

    // Display splash
    auto& splash = host_window.open_overlay<EssaSplash>(gui.settings_gui());
    splash.show_modal();

    application.run();
    return EXIT_SUCCESS;
}
