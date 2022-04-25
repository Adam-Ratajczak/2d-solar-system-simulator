#include "ModifyObject.hpp"

ModifyObject::ModifyObject(GUI::Container& c) : GUI::Container(c){
    set_layout<GUI::VerticalBoxLayout>().set_spacing(10);

    auto main_label = add_widget<GUI::Textfield>();
    main_label->set_content("Modify object");
    main_label->set_alignment(GUI::Align::Center);
    main_label->set_size({Length::Auto, 30.0_px});

    auto mass_container = add_widget<GUI::Container>();
    mass_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(5);
    mass_container->set_size({Length::Auto, 30.0_px});

    auto mass_label = mass_container->add_widget<GUI::Textfield>();
    mass_label->set_content("Mass: ");
    mass_label->set_alignment(GUI::Align::CenterLeft);
    mass_label->set_size({150.0_px, Length::Auto});
    
    m_mass_textbox = mass_container->add_widget<GUI::Textbox>();
    m_mass_textbox->set_size({150.0_px, Length::Auto});

    auto m_mass_exponent_label = mass_container->add_widget<GUI::Textfield>();
    m_mass_exponent_label->set_content(" * 10 ^ ");
    m_mass_exponent_label->set_alignment(GUI::Align::Center);

    m_mass_exponent_textbox = mass_container->add_widget<GUI::Textbox>();

    auto radius_container = add_widget<GUI::Container>();
    radius_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(5);
    radius_container->set_size({Length::Auto, 30.0_px});

    auto radius_label = radius_container->add_widget<GUI::Textfield>();
    radius_label->set_content("Radius: ");
    radius_label->set_alignment(GUI::Align::CenterLeft);
    radius_label->set_size({150.0_px, Length::Auto});
    
    m_radius_textbox = radius_container->add_widget<GUI::Textbox>();

    auto velocity_container = add_widget<GUI::Container>();
    velocity_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(5);
    velocity_container->set_size({Length::Auto, 30.0_px});

    auto velocity_label = velocity_container->add_widget<GUI::Textfield>();
    velocity_label->set_content("Velocity: ");
    velocity_label->set_alignment(GUI::Align::CenterLeft);
    velocity_label->set_size({150.0_px, Length::Auto});

    m_velocity_control = add_widget<GUI::ValueSlider>(0, 100000);
    m_velocity_control->set_unit("m / s");
    m_velocity_control->set_name("Velocity");

    auto direction_xz_container = add_widget<GUI::Container>();
    direction_xz_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(5);
    direction_xz_container->set_size({Length::Auto, 30.0_px});

    m_direction_xz_control = add_widget<GUI::ValueSlider>(0, 360);
    m_direction_xz_control->set_unit("[deg]");
    m_direction_xz_control->set_name("Direction");

    auto direction_yz_container = add_widget<GUI::Container>();
    direction_yz_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(5);
    direction_yz_container->set_size({Length::Auto, 30.0_px});

    m_direction_yz_control = add_widget<GUI::ValueSlider>(0, 360);
    m_direction_yz_control->set_unit("[deg]");
    m_direction_yz_control->set_name("Tilt");

    auto main_color_container = add_widget<Container>();
    main_color_container->set_size({ Length::Auto, 150.0_px });
    main_color_container->set_layout<GUI::VerticalBoxLayout>().set_spacing(10);
    {
        auto color_label_textfield = main_color_container->add_widget<GUI::Textfield>();
        color_label_textfield->set_content("COLOR");
        color_label_textfield->set_alignment(GUI::Align::Center);

        m_color_control = main_color_container->add_widget<GUI::ColorPicker>();
        m_color_control->set_size({ Length::Auto, 100.0_px });
    }
    auto name_container = add_widget<Container>();
    name_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    name_container->set_size({Length::Auto, 30.0_px});
    {
        auto name_textfield = name_container->add_widget<GUI::Textfield>();
        name_textfield->set_size({ 150.0_px, Length::Auto });
        name_textfield->set_content("Name: ");
        name_textfield->set_alignment(GUI::Align::CenterLeft);

        m_name_textbox = name_container->add_widget<GUI::Textbox>();
        m_name_textbox->set_limit(20);
        m_name_textbox->set_data_type(GUI::Textbox::TEXT);
    }
}
