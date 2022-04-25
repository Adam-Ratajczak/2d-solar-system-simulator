#pragma once
#include "../gui/Container.hpp"
#include "../gui/Textbox.hpp"
#include "../gui/ValueSlider.hpp"
#include "../gui/ColorPicker.hpp"
#include "../Object.hpp"
#include <SFML/Graphics/Color.hpp>

class ModifyObject : public GUI::Container{
public:
    explicit ModifyObject(GUI::Container& c);

    void refresh(Object* obj);
    void resore_defaults();

private:
    Object* m_object = nullptr;

    float m_mass, m_mass_exponent, m_speed;
    Distance m_radius;
    Angle m_direction_xz, m_direction_yz;
    sf::Color m_color;
    std::string m_name;

    GUI::Textbox* m_mass_textbox = nullptr;
    GUI::Textbox* m_mass_exponent_textbox = nullptr;
    GUI::Textbox* m_radius_textbox = nullptr;
    GUI::Textbox* m_name_textbox = nullptr;

    GUI::ValueSlider* m_velocity_control = nullptr;
    GUI::ValueSlider* m_direction_xz_control = nullptr;
    GUI::ValueSlider* m_direction_yz_control = nullptr;

    GUI::ColorPicker* m_color_control = nullptr;
};
