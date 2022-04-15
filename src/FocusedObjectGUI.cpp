#include "EssaGUI.hpp"
#include "Object.hpp"
#include "gui/Container.hpp"
#include "gui/Textfield.hpp"
#include <memory>

std::shared_ptr<GUI::Container> EssaGUI::m_create_focused_object_info_gui(){
    auto container = add_widget<GUI::Container>();

    auto& layout = container->set_layout<GUI::VerticalBoxLayout>();
    layout.set_spacing(5);
    container->set_position({10.0_px_o, 10.0_px});
    container->set_size({400.0_px, 300.0_px});

    auto subcontainer = container->add_widget<GUI::Container>();
    subcontainer->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    subcontainer->add_widget<GUI::Textfield>()->set_content("Mass: ").set_size({150.0_px, Length::Auto});
    m_info_content.push_back(subcontainer->add_widget<GUI::Textfield>());                   // mass value
    subcontainer->add_widget<GUI::Textfield>()->set_content(" * 10 ^ ");                    // scientific notation
    m_info_content.push_back(subcontainer->add_widget<GUI::Textfield>());                   // exponent
    subcontainer->add_widget<GUI::Textfield>()->set_content("kg");                          // unit

    subcontainer = container->add_widget<GUI::Container>();
    subcontainer->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    subcontainer->add_widget<GUI::Textfield>()->set_content("Radius: ").set_size({150.0_px, Length::Auto});
    m_info_content.push_back(subcontainer->add_widget<GUI::Textfield>());                   // radius value
    subcontainer->add_widget<GUI::Textfield>()->set_content("km");                          // unit

    subcontainer = container->add_widget<GUI::Container>();
    subcontainer->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    subcontainer->add_widget<GUI::Textfield>()->set_content("Velocity: ").set_size({150.0_px, Length::Auto});
    m_info_content.push_back(subcontainer->add_widget<GUI::Textfield>());                   // velocity value
    subcontainer->add_widget<GUI::Textfield>()->set_content("m / s");                       // unit

    subcontainer = container->add_widget<GUI::Container>();
    subcontainer->set_name("Distance");
    subcontainer->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    subcontainer->add_widget<GUI::Textfield>()->set_content("Distance: ").set_size({150.0_px, Length::Auto});
    m_info_content.push_back(subcontainer->add_widget<GUI::Textfield>());                   // distance value
    subcontainer->add_widget<GUI::Textfield>()->set_content("AU");                          // unit

    subcontainer = container->add_widget<GUI::Container>();
    subcontainer->set_name("Apogee");
    subcontainer->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    subcontainer->add_widget<GUI::Textfield>()->set_content("Apogee: ").set_size({150.0_px, Length::Auto});
    m_info_content.push_back(subcontainer->add_widget<GUI::Textfield>());                   // apogee value
    subcontainer->add_widget<GUI::Textfield>()->set_content("AU");                          // unit

    subcontainer = container->add_widget<GUI::Container>();
    subcontainer->set_name("Apogee_vel");
    subcontainer->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    subcontainer->add_widget<GUI::Textfield>()->set_content("Velocity at apogee: ").set_size({150.0_px, Length::Auto});
    m_info_content.push_back(subcontainer->add_widget<GUI::Textfield>());                   // apogee vel value
    subcontainer->add_widget<GUI::Textfield>()->set_content("m / s");                       // unit

    subcontainer = container->add_widget<GUI::Container>();
    subcontainer->set_name("Perigee");
    subcontainer->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    subcontainer->add_widget<GUI::Textfield>()->set_content("Perigee: ").set_size({150.0_px, Length::Auto});
    m_info_content.push_back(subcontainer->add_widget<GUI::Textfield>());                   // perigee value
    subcontainer->add_widget<GUI::Textfield>()->set_content("AU");                          // unit

    subcontainer = container->add_widget<GUI::Container>();
    subcontainer->set_name("Perigee_vel");
    subcontainer->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    subcontainer->add_widget<GUI::Textfield>()->set_content("Velocity at perigee: ").set_size({150.0_px, Length::Auto});
    m_info_content.push_back(subcontainer->add_widget<GUI::Textfield>());                   // perigee vel value
    subcontainer->add_widget<GUI::Textfield>()->set_content("m / s");                       // unit

    subcontainer = container->add_widget<GUI::Container>();
    subcontainer->set_name("Period");
    subcontainer->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    subcontainer->add_widget<GUI::Textfield>()->set_content("Period: ").set_size({150.0_px, Length::Auto});
    m_info_content.push_back(subcontainer->add_widget<GUI::Textfield>());                   // period value
    subcontainer->add_widget<GUI::Textfield>()->set_content("years");                       // unit

    subcontainer = container->add_widget<GUI::Container>();
    subcontainer->set_name("Eccencrity");
    subcontainer->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    subcontainer->add_widget<GUI::Textfield>()->set_content("Eccencrity: ").set_size({150.0_px, Length::Auto});
    m_info_content.push_back(subcontainer->add_widget<GUI::Textfield>());                   // eccenctrity value

    return container;
}

void EssaGUI::m_switch_info(bool state){
    m_focused_object_info->find_widget("Distance")->set_visible(state);
    m_focused_object_info->find_widget("Apogee")->set_visible(state);
    m_focused_object_info->find_widget("Apogee_vel")->set_visible(state);
    m_focused_object_info->find_widget("Perigee")->set_visible(state);
    m_focused_object_info->find_widget("Perigee_vel")->set_visible(state);
    m_focused_object_info->find_widget("Period")->set_visible(state);
    m_focused_object_info->find_widget("Eccencrity")->set_visible(state);
}

void EssaGUI::m_update_focused_object_info_gui(Object* focused){
    auto info = focused->get_info();
    for(unsigned i = 0; i < info.size(); i++){
        m_info_content[i]->set_content(info[i]);
    }

    if(focused == m_world.most_massive_object()){
        m_focused_object_info->set_size({400.0_px, 90.0_px});
        m_switch_info(false);
    }else{
        m_focused_object_info->set_size({400.0_px, 300.0_px});
        m_switch_info(true);
    }
}
