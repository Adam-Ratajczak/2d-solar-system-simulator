#pragma once

#include "Object.hpp"
#include "gui/Container.hpp"
#include "gui/Textfield.hpp"

class FocusedObjectGUI : public GUI::Container {
public:
    explicit FocusedObjectGUI(GUI::Container&);

    void update_from_object(Object*);

private:
    void set_most_massive_data_visible(bool);

    GUI::Textfield* m_title = nullptr;

    GUI::Textfield* m_mass_textfield = nullptr;
    GUI::Textfield* m_radius_textfield = nullptr;
    GUI::Textfield* m_absolute_velocity_textfield = nullptr;

    GUI::Textfield* m_orbiting_title = nullptr;

    GUI::Textfield* m_distance_from_most_massive_object_textfield = nullptr;
    GUI::Textfield* m_apogee_textfield = nullptr;
    GUI::Textfield* m_apogee_velocity_textfield = nullptr;
    GUI::Textfield* m_perigee_textfield = nullptr;
    GUI::Textfield* m_perigee_velocity_textfield = nullptr;
    GUI::Textfield* m_orbit_period_textfield = nullptr;
    GUI::Textfield* m_orbit_eccencrity_textfield = nullptr;
};
