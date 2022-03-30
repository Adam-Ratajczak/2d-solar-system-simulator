#pragma once

#include "Container.hpp"
#include "Slider.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"

class ValueSlider : public Container
{
public:
    ValueSlider(Container& parent, double min, double max, double step = 1);

    void set_name(std::string name) { m_name_textfield->set_content(std::move(name)); }
    void set_unit(std::string unit) { m_unit_textfield->set_content(std::move(unit)); }

    double value() const;

    std::function<void(double)> on_change;

private:
    std::shared_ptr<Textfield> m_name_textfield;
    std::shared_ptr<Slider> m_slider;
    std::shared_ptr<Textbox> m_textbox;
    std::shared_ptr<Textfield> m_unit_textfield;
};
