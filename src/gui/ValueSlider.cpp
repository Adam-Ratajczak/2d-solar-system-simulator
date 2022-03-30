#include "ValueSlider.hpp"

#include <iostream>

ValueSlider::ValueSlider(Container& parent, double min, double max, double step)
: Container(parent)
{
    auto& layout = set_layout<HorizontalBoxLayout>();
    layout.set_spacing(5);
    m_name_textfield = add_widget<Textfield>();
    m_name_textfield->set_size({100.0_px, Length::Auto});
    m_textbox = add_widget<Textbox>();
    m_textbox->set_data_type(Textbox::NUMBER);
    // TODO: Textbox on_change
    m_slider = add_widget<Slider>(min, max, step);
    m_slider->on_change = [&](double value) {
        m_textbox->set_content(std::to_string(value));
        if(on_change)
            on_change(value);
    };
    m_unit_textfield = add_widget<Textfield>();
    m_unit_textfield->set_size({50.0_px, Length::Auto});
}

double ValueSlider::value() const
{
    return m_slider->get_value();
}
