#include "EssaSplash.hpp"

#include "EssaGUI.hpp"
#include "EssaSettings.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Overlays/FilePrompt.hpp>
#include <Essa/GUI/Overlays/MessageBox.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/ImageWidget.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>

#include <algorithm>

EssaSplash::EssaSplash(GUI::HostWindow& window, EssaSettings& essa_settings)
    : GUI::ToolWindow(window)
    , m_essa_settings(essa_settings) {
    (void)load_from_eml_resource(resource_manager().require<EML::EMLResource>("Splash.eml"));

    auto main_widget = static_cast<GUI::Container*>(this->main_widget());
    main_widget->find_widget_of_type_by_id_recursively<GUI::Button>("open_solar_system")->on_click = [this]() {
        m_essa_settings.load_world("../worlds/solar.essa");
        close();
    };
    main_widget->find_widget_of_type_by_id_recursively<GUI::Button>("open_file")->on_click = [this]() {
        auto& prompt = host_window().open_overlay<GUI::FilePrompt>("Choose file to open: ", "Open file", "e.g.: solar.essa");
        prompt.add_desired_extension(".essa");
        prompt.show_modal();

        if (prompt.result().has_value()) {
            m_essa_settings.load_world(prompt.result().value().encode());
            close();
        }
    };
    main_widget->find_widget_of_type_by_id_recursively<GUI::Button>("create_empty")->on_click = [this]() {
        close();
    };
    main_widget->find_widget_of_type_by_id_recursively<GUI::Button>("about")->on_click = [this]() {
        constexpr char const AboutString[] = "ESSA - Extremely Sophiscated Space Application\n"
                                             "\n"
                                             "Copyright (c) 2022 Adam2004yt, sppmacd\n"
                                             "\n"
                                             "Licensed under Beerware License\n"
                                             "Source: https://github.com/essa-software/essa\n"
                                             "\n"
                                             "Powered by:\n"
                                             "\u2022 EssaGUI (https://github.com/essa-software/EssaGUI)\n"
                                             "\u2022 EssaUtil (https://github.com/essa-software/util)\n"
                                             "\u2022 Python (https://github.com/python/cpython";

        GUI::message_box(host_window(), AboutString, "About", GUI::MessageBox::Buttons::Ok);
    };
}

void EssaSplash::handle_event(GUI::Event const& event) {
    GUI::ToolWindow::handle_event(event);

    if (auto e = event.get<GUI::Event::MouseButtonPress>(); e) {
        if (!rect().contains(e->local_position())) {
            close();
        }
    }
    else if (auto e = event.get<GUI::Event::KeyPress>(); e) {
        if (e->code() == llgl::KeyCode::Escape) {
            close();
        }
    }
}
