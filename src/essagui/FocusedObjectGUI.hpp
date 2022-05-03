#pragma once

#include "../Object.hpp"
#include "../gui/Container.hpp"
#include "../gui/ColorPicker.hpp"
#include "../gui/Textfield.hpp"
#include "../gui/ValueSlider.hpp"
#include "../util/UnitDisplay.hpp"

class FocusedObjectGUI : public GUI::Container {
public:
    explicit FocusedObjectGUI(GUI::WidgetTreeRoot& c, Object* o);

    virtual void update() override;

private:
    void set_most_massive_data_visible(bool);

    Object* m_focused = nullptr;

    GUI::Textfield* m_orbiting_title = nullptr;

    struct Field {
        GUI::Textfield* unit_textfield { nullptr };
        GUI::Textfield* value_textfield { nullptr };

        void set_content_from_unit_value(Util::UnitValue const&) const;
    };

    Field m_mass_textfield;
    Field m_radius_textfield;
    Field m_absolute_velocity_textfield;

    Field m_distance_from_most_massive_object_textfield;
    Field m_apogee_textfield;
    Field m_apogee_velocity_textfield;
    Field m_perigee_textfield;
    Field m_perigee_velocity_textfield;
    Field m_orbit_period_textfield;
    Field m_orbit_eccencrity_textfield;

    GUI::ValueSlider* m_radius_control = nullptr;
    GUI::ValueSlider* m_velocity_control = nullptr;
    GUI::ValueSlider* m_direction_xz_control = nullptr;
    GUI::ValueSlider* m_direction_yz_control = nullptr;
    GUI::ValueSlider* m_y_position_control = nullptr;

    GUI::Textbox* m_mass_textbox = nullptr;
    GUI::Textbox* m_mass_exponent_textbox = nullptr;
    GUI::Textbox* m_name_textbox = nullptr;

    GUI::ColorPicker* m_color_control = nullptr;

    Vector3 m_new_object_pos;

    void m_create_info_gui(GUI::Container& info);
    void m_create_modify_gui(GUI::Container& modify);
};
