#include "EssaSplash.hpp"

#include "EssaSettings.hpp"

#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/MessageBox.hpp>
#include <EssaGUI/gui/TextButton.hpp>
#include <EssaGUI/gui/Textfield.hpp>
#include <EssaGUI/gui/ToolWindow.hpp>

#include <algorithm>

EssaSplash::EssaSplash(sf::RenderWindow& wnd, EssaSettings& essa_settings)
    : GUI::ToolWindow(wnd)
    , m_essa_settings(essa_settings) {
    set_title("ESSA");
    set_size({ 600, 500 });
    center_on_screen();

    auto& container = set_main_widget<GUI::Container>();
    auto& container_layout = container.set_layout<GUI::VerticalBoxLayout>();
    container_layout.set_padding(10);
    container_layout.set_spacing(10);

    // TODO: Make it an image
    auto logo_label = container.add_widget<GUI::Textfield>();
    logo_label->set_size({ Length::Auto, 160.0_px });
    logo_label->set_content("ESSA");
    logo_label->set_font_size(50);
    logo_label->set_alignment(GUI::Align::Center);

    auto button_space = container.add_widget<GUI::Container>();
    auto& button_space_layout = button_space->set_layout<GUI::HorizontalBoxLayout>();
    button_space_layout.set_padding(10);
    button_space_layout.set_spacing(10);

    auto add_button = [](GUI::Container* container, sf::String label, std::function<void()> on_click) {
        auto button = container->add_widget<GUI::TextButton>();
        button->set_size({ Length::Auto, 50.0_px });
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

    add_button(button_space_column1, "Empty world", [this]() {
        close();
    });

    auto button_space_column2 = button_space->add_widget<GUI::Container>();
    auto& button_space_column2_layout = button_space_column2->set_layout<GUI::VerticalBoxLayout>();
    button_space_column2_layout.set_spacing(5);

    add_button(button_space_column2, "About...", [this]() {
        constexpr char const AboutString[] = "ESSA - Extremely Sophiscated Space Application\n"
                                             "\n"
                                             "Copyright (c) 2022 Adam2004yt, sppmacd\n"
                                             "\n"
                                             "Licensed under Beerware License\n"
                                             "Source: https://github.com/Adam-Ratajczak/essa\n"
                                             "\n"
                                             "\u2022 EssaGUI (https://github.com/sppmacd/EssaGUI)\n"
                                             "\u2022 SFML (https://github.com/SFML/SFML)\n"
                                             "\u2022 Python (https://github.com/python/cpython";

        GUI::message_box(sf::String::fromUtf8(std::begin(AboutString), std::end(AboutString) - 1), "About", GUI::MessageBox::Buttons::Ok);
    });
}

void EssaSplash::handle_event(sf::Event event) {
    GUI::ToolWindow::handle_event(event);

    GUI::Event gui_event { event };
    if (gui_event.type() == sf::Event::MouseButtonPressed && !full_rect().contains(gui_event.mouse_position() + position()))
        close();
}
