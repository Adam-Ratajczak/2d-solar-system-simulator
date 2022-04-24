#pragma once

#include "../Object.hpp"
#include "../gui/Container.hpp"
#include "../gui/Textfield.hpp"
#include "../util/UnitDisplay.hpp"

class FocusedObjectGUI : public GUI::Container {
public:
    explicit FocusedObjectGUI(GUI::Container&);

    void update_from_object(Object*);

private:
    void set_most_massive_data_visible(bool);

    GUI::Textfield* m_title = nullptr;
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
};
