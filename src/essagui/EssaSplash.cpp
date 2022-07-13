#include "EssaSplash.hpp"

#include "EssaGUI.hpp"
#include "EssaSettings.hpp"

#include <EssaGUI/gfx/ResourceLoader.hpp>
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/FilePrompt.hpp>
#include <EssaGUI/gui/ImageWidget.hpp>
#include <EssaGUI/gui/MessageBox.hpp>
#include <EssaGUI/gui/TextButton.hpp>
#include <EssaGUI/gui/Textfield.hpp>
#include <EssaGUI/gui/ToolWindow.hpp>

#include <algorithm>

EssaSplash::EssaSplash(GUI::SFMLWindow& wnd, EssaSettings& essa_settings)
    : GUI::ToolWindow(wnd)
    , m_essa_settings(essa_settings) {
    set_title("ESSA");
    set_size({ 600, 500 });
    center_on_screen();

    auto& container = set_main_widget<GUI::Container>();
    auto& container_layout = container.set_layout<GUI::VerticalBoxLayout>();
    container_layout.set_padding(10);
    container_layout.set_spacing(10);

    static sf::Texture logo_image = Gfx::require_texture("../assets/splash_full_v1.png");
    logo_image.setSmooth(true);

    auto logo = container.add_widget<GUI::ImageWidget>();
    logo->set_size({ Length::Auto, 160.0_px });
    logo->set_image(&logo_image);

    auto button_space = container.add_widget<GUI::Container>();
    auto& button_space_layout = button_space->set_layout<GUI::HorizontalBoxLayout>();
    button_space_layout.set_padding(10);
    button_space_layout.set_spacing(20);

    auto add_button = [](GUI::Container* container, Util::UString label, std::function<void()> on_click) {
        auto button = container->add_widget<GUI::TextButton>();
        button->set_size({ Length::Auto, 40.0_px });
        button->set_content(std::move(label));
        button->on_click = std::move(on_click);
    };

    auto button_space_column1 = button_space->add_widget<GUI::Container>();
    auto& button_space_column1_layout = button_space_column1->set_layout<GUI::VerticalBoxLayout>();
    button_space_column1_layout.set_spacing(10);

    add_button(button_space_column1, "Open Solar System", [this]() {
        m_essa_settings.load_world("../worlds/solar.essa");
        close();
    });

    add_button(button_space_column1, "Open file", [this]() {
        auto& prompt = GUI::Application::the().open_overlay<GUI::FilePrompt>("Choose file to open: ", "Open file", "e.g.: solar.essa");
        prompt.add_desired_extension(".essa");
        prompt.run();

        if (prompt.result().has_value())
            m_essa_settings.load_world(prompt.result().value().encode());
        close();
    });

    add_button(button_space_column1, "Empty world", [this]() {
        close();
    });

    auto button_space_column2 = button_space->add_widget<GUI::Container>();
    auto& button_space_column2_layout = button_space_column2->set_layout<GUI::VerticalBoxLayout>();
    button_space_column2_layout.set_spacing(10);

    add_button(button_space_column2, "About...", [this]() {
        constexpr char const AboutString[] = "ESSA - Extremely Sophiscated Space Application\n"
                                             "\n"
                                             "Copyright (c) 2022 Adam2004yt, sppmacd\n"
                                             "\n"
                                             "Licensed under Beerware License\n"
                                             "Source: https://github.com/essa-software/essa\n"
                                             "\n"
                                             "Powered by:\n"
                                             "\u2022 EssaGUI (https://github.com/essa-software/EssaGUI)\n"
                                             "\u2022 SFML (https://github.com/SFML/SFML)\n"
                                             "\u2022 Python (https://github.com/python/cpython";

        GUI::message_box(AboutString, "About", GUI::MessageBox::Buttons::Ok);
    });
}

void EssaSplash::handle_event(sf::Event event) {
    GUI::ToolWindow::handle_event(event);

    GUI::Event gui_event { event };
    if (gui_event.type() == sf::Event::MouseButtonPressed && !full_rect().contains(gui_event.mouse_position() + position())
        || gui_event.type() == sf::Event::KeyPressed && gui_event.event().key.code == sf::Keyboard::Escape)
        close();
}
