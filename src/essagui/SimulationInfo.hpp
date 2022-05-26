#pragma once

#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/Textfield.hpp>
#include <SFML/System/Clock.hpp>

#include "../SimulationView.hpp"

class SimulationInfo : public GUI::Container {
public:
    explicit SimulationInfo(GUI::Container& parent, SimulationView* sw);

    float get_fps() const{return m_fps;}

    virtual void do_update() override;

private:
    sf::Clock m_clock;
    float m_fps = 60;

    void m_update_time();
    void m_update_fps();

    GUI::Textfield* m_fps_field = nullptr;
    GUI::Textfield* m_time_field = nullptr;
    SimulationView* m_simulation_view = nullptr;
};
