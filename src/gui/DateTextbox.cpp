#include "DateTextbox.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"

namespace GUI {
DateTextbox::DateTextbox(Container& parent) : Container(parent), m_date(0){
    set_layout<HorizontalBoxLayout>().set_spacing(10);
    
    auto days = add_widget<Textbox>();
    days->set_data_type(Textbox::Type::NUMBER);
    days->set_limit(2);
    days->set_min_max_values(1, 31);

    add_widget<Textfield>()->set_content(" / ");
    
    auto months = add_widget<Textbox>();
    months->set_data_type(Textbox::Type::NUMBER);
    months->set_limit(2);
    months->set_min_max_values(1, 12);

    add_widget<Textfield>()->set_content(" / ");
    
    auto years = add_widget<Textbox>();
    years->set_data_type(Textbox::Type::NUMBER);
    years->set_limit(4);
    years->set_min_max_values(1, 9999);
}

}