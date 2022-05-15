#include "MessageBox.hpp"

#include "Container.hpp"
#include "TextButton.hpp"
#include "Textfield.hpp"

namespace GUI {

MessageBox::MessageBox(sf::RenderWindow& wnd, sf::String message, sf::String title, Buttons buttons)
    : ToolWindow(wnd, "MessageBox") {
    // FIXME: Actually measure text width instead of throwing up random number
    set_size({ static_cast<float>(message.getSize() * 12), 150 });
    set_title(std::move(title));

    auto& prompt_container = set_main_widget<GUI::Container>();
    prompt_container.set_background_color(sf::Color::White);

    auto& prompt_container_layout = prompt_container.set_layout<GUI::VerticalBoxLayout>();
    prompt_container_layout.set_spacing(20);
    prompt_container_layout.set_padding(20);

    auto prompt_text = prompt_container.add_widget<GUI::Textfield>();
    prompt_text->set_content(std::move(message));
    prompt_text->set_alignment(GUI::Align::Center);
    prompt_text->set_display_attributes(sf::Color::Transparent, sf::Color::Transparent, sf::Color::Black);

    auto button_container = prompt_container.add_widget<GUI::Container>();
    button_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(20);
    button_container->set_size({ Length::Auto, 30.0_px });

    auto add_button = [this, &button_container](ButtonRole button_role, sf::String label, sf::Color bg_color) {
        auto button = button_container->add_widget<GUI::TextButton>();
        button->set_alignment(GUI::Align::Center);
        button->set_content(std::move(label));
        button->set_display_attributes(bg_color, sf::Color::Blue, sf::Color::White);
        button->on_click = [this, button_role]() {
            m_clicked_button = button_role;
            close();
        };
    };

    if (buttons == Buttons::YesNo) {
        add_button(ButtonRole::Yes, "Yes", sf::Color(100, 200, 100));
        add_button(ButtonRole::No, "No", sf::Color(100, 100, 100));
    }else if (buttons == Buttons::Ok) {
        add_button(ButtonRole::Ok, "Ok", sf::Color(0, 0, 255));
    }
}

}
