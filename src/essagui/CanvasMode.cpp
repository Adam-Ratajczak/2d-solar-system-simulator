#include "EssaGUI.hpp"
#include "../gui/ArrowButton.hpp"
#include "../gui/Container.hpp"
#include "../gui/Datebox.hpp"
#include "../gui/TextAlign.hpp"
#include "../gui/Textfield.hpp"
#include "../util/Units.hpp"
#include <SFML/Graphics/Color.hpp>
#include <memory>

void EssaGUI::m_create_cavas_mode_gui(Container& parent) {
    auto sizes = m_simulation_view->window().getSize();
    parent.set_size({ { static_cast<float>(sizes.x - 100), Length::Px }, { static_cast<float>(sizes.y - 10), Length::Px } });
    parent.set_layout<GUI::VerticalBoxLayout>().set_spacing(20);

    auto main_label = parent.add_widget<GUI::Textfield>();
    main_label->set_content("Canvas Mode");
    main_label->set_alignment(GUI::Align::Center);
    main_label->set_size({ Length::Auto, 30.0_px });

    auto main_layout = parent.add_widget<GUI::Container>();
    main_layout->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);

    auto left_panel = main_layout->add_widget<GUI::Container>();
    left_panel->set_layout<GUI::VerticalBoxLayout>().set_spacing(10);
    left_panel->set_size({ 500.0_px, Length::Auto });

    auto date = left_panel->add_widget<GUI::Datebox>();
    date->set_size({ Length::Auto, 40.0_px });
    date->set_display_attributes(sf::Color::White, sf::Color::Blue, sf::Color::Black);

    auto arrow_box = left_panel->add_widget<GUI::Container>();
    arrow_box->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    arrow_box->set_size({ Length::Auto, 30.0_px });
    auto arrow1 = arrow_box->add_widget<GUI::ArrowButton>();
    arrow1->set_arrow_color(sf::Color::Black);
    arrow1->set_arrow_type(GUI::ArrowButton::ArrowType::LEFTARROW);
    arrow1->set_arrow_size(20);
    auto arrow2 = arrow_box->add_widget<GUI::ArrowButton>();
    arrow2->set_arrow_color(sf::Color::Black);
    arrow2->set_arrow_type(GUI::ArrowButton::ArrowType::RIGHTARROW);
    arrow2->set_arrow_size(20);

    auto simulation_panel = main_layout->add_widget<GUI::Container>();
    simulation_panel->set_layout<GUI::VerticalBoxLayout>().set_spacing(10);
    // simulation_panel->set_size({500.0_px, Length::Auto});
    simulation_panel->set_background_color(sf::Color::Blue);

    auto object_list_panel = main_layout->add_widget<GUI::Container>();
    object_list_panel->set_layout<GUI::VerticalBoxLayout>().set_spacing(10);
    object_list_panel->set_size({ 300.0_px, Length::Auto });

    auto bottom_container = parent.add_widget<GUI::Container>();
    bottom_container->set_size({ Length::Auto, 100.0_px });
}
