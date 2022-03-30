#include "ValueSlider.hpp"
#include "NotifyUser.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

static std::string serialize_value(double value, double step) {
    std::ostringstream oss;
    oss << std::fixed;
    if (step < 1)
        oss << std::setprecision(std::ceil(-std::log10(step)));
    else
        oss << std::setprecision(0);
    oss << value;
    return oss.str();
}

ValueSlider::ValueSlider(Container& parent, double min, double max, double step)
    : Container(parent) {

    auto& layout = set_layout<HorizontalBoxLayout>();
    layout.set_spacing(5);
    m_name_textfield = add_widget<Textfield>();
    m_name_textfield->set_size({ 100.0_px, Length::Auto });
    m_slider = add_widget<Slider>(min, max, step);
    m_slider->on_change = [this](double value) {
        m_textbox->set_content(serialize_value(value, m_slider->step()), NotifyUser::No);
        if (on_change)
            on_change(value);
    };
    m_textbox = add_widget<Textbox>();
    m_textbox->set_data_type(Textbox::NUMBER);
    m_textbox->set_content(serialize_value(m_slider->get_value(), m_slider->step()));
    m_textbox->on_change = [this](std::string const& value) {
        try {
            m_slider->set_value(std::stod(value), NotifyUser::No);
        } catch (...) { ; }
    };
    m_unit_textfield = add_widget<Textfield>();
    m_unit_textfield->set_size({ 50.0_px, Length::Auto });
}

double ValueSlider::value() const {
    return m_slider->get_value();
}
