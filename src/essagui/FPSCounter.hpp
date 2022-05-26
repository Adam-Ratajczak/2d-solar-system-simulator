#pragma once

#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/Textfield.hpp>
#include <SFML/System/Clock.hpp>
class FPSCounter : public GUI::Container {
public:
    explicit FPSCounter(GUI::Container& parent);

    float get_fps() const{return m_fps;}

    virtual void do_update() override;

private:
    sf::Clock m_clock;
    float m_fps = 60;

    GUI::Textfield *m_field;
};
