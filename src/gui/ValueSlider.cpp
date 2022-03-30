#include "ValueSlider.hpp"
#include "NotifyUser.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

ValueSlider::ValueSlider(Container& parent, double min, double max, double step)
    : Container(parent) {
    auto& layout = set_layout<HorizontalBoxLayout>();
    layout.set_spacing(5);
    m_name_textfield = add_widget<Textfield>();
    m_name_textfield->set_size({ 100.0_px, Length::Auto });
    m_slider = add_widget<Slider>(min, max, step);
    m_slider->on_change = [&](double value) {
        std::ostringstream oss;
        oss << std::fixed;
        if(m_slider->step() < 1)
            oss << std::setprecision(std::ceil(std::log10(m_slider->step())));
        else
            oss << std::setprecision(0);
        oss << value;
        m_textbox->set_content(oss.str(), NotifyUser::No);
        if (on_change)
            on_change(value);
    };
    m_textbox = add_widget<Textbox>();
    m_textbox->set_data_type(Textbox::NUMBER);
    m_textbox->set_content("0");
    m_textbox->on_change = [&](std::string const& value) {
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
