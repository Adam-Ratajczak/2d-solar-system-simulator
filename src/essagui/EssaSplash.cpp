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

EssaSplash::EssaSplash(GUI::WidgetTreeRoot& window, EssaSettings& essa_settings)
    : GUI::WindowRoot(window)
    , m_essa_settings(essa_settings) {
    (void)load_from_eml_resource(GUI::Application::the().resource_manager().require<EML::EMLResource>("Splash.eml"));

    auto main_widget = static_cast<GUI::Container*>(this->main_widget());
    main_widget->find_widget_of_type_by_id_recursively<GUI::Button>("open_solar_system")->on_click = [this]() {
        m_essa_settings.load_world("../worlds/solar.essa");
        close();
    };
    main_widget->find_widget_of_type_by_id_recursively<GUI::Button>("open_file")->on_click = [this]() {
        auto prompt = GUI::Application::the().open_host_window<GUI::FilePrompt>("Choose file to open: ", "Open file", "e.g.: solar.essa");
        prompt.root.add_desired_extension(".essa");
        prompt.window.show_modal();

        if (prompt.root.result().has_value()) {
            m_essa_settings.load_world(prompt.root.result().value().encode());
            close();
        }
    };
    main_widget->find_widget_of_type_by_id_recursively<GUI::Button>("create_empty")->on_click = [this]() {
        close();
    };
    main_widget->find_widget_of_type_by_id_recursively<GUI::Button>("about")->on_click = [main_widget]() {
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

        // FIXME: Taking host window from random widget
        GUI::message_box(main_widget->host_window(), AboutString, "About", GUI::MessageBox::Buttons::Ok);
    };
}

GUI::Widget::EventHandlerResult EssaSplash::handle_event(GUI::Event const& event) {
    if (event.is<GUI::Event::WindowFocusLost>()) {
        close();
        return GUI::Widget::EventHandlerResult::Accepted;
    }
    else if (auto e = event.get<GUI::Event::KeyPress>(); e) {
        if (e->code() == llgl::KeyCode::Escape) {
            close();
            return GUI::Widget::EventHandlerResult::Accepted;
        }
    }
    return GUI::Widget::EventHandlerResult::NotAccepted;
}
