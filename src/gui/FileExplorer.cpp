#include "FileExplorer.hpp"
#include "Container.hpp"
#include "Application.hpp"
#include "MessageBox.hpp"
#include "NotifyUser.hpp"
#include "TextButton.hpp"
#include "Textbox.hpp"
#include "ListView.hpp"
#include <SFML/Graphics/Color.hpp>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace GUI {

FileExplorer::FileExplorer(Container& c) : Container(c){
    set_layout<VerticalBoxLayout>();
    m_current_path = std::filesystem::absolute("../worlds");

    auto toolbar = add_widget<Container>();
    toolbar->set_layout<HorizontalBoxLayout>();
    toolbar->set_size({Length::Auto, 30.0_px});

    auto path_textbox = toolbar->add_widget<Textbox>();
    path_textbox->set_data_type(Textbox::Type::TEXT);
    path_textbox->set_content(m_current_path.string());
    path_textbox->set_size({{50.0, Length::Percent}, Length::Auto});

    auto parent_directory_button = toolbar->add_widget<TextButton>();
    parent_directory_button->set_content("Parent");
    parent_directory_button->set_alignment(Align::Center);
    parent_directory_button->set_background_color(sf::Color::Green);

    auto create_directory_button = toolbar->add_widget<TextButton>();
    create_directory_button->set_content("Create folder");
    create_directory_button->set_alignment(Align::Center);
    create_directory_button->set_background_color(sf::Color::Blue);
    create_directory_button->on_click = [&](){
        
    };

    auto create_file_button = toolbar->add_widget<TextButton>();
    create_file_button->set_content("Create file");
    create_file_button->set_alignment(Align::Center);
    create_file_button->set_background_color(sf::Color::Blue);
    create_file_button->on_click = [&](){
        
    };

    auto find = toolbar->add_widget<Textbox>();
    find->set_placeholder("Find file or directory");
    find->set_size({{25.0, Length::Percent}, Length::Auto});

    auto main_container = add_widget<Container>();
    main_container->set_layout<HorizontalBoxLayout>().set_spacing(10);

    auto sidebar = main_container->add_widget<Container>();
    sidebar->set_size({{20.0, Length::Percent}, Length::Auto});

    auto list = main_container->add_widget<ListView>();
    m_model = &list->create_and_set_model<FileView>();
    m_model->update_content(m_current_path);

    path_textbox->on_enter = [&, path_textbox](std::string path) mutable{
        if(std::filesystem::exists(path) && std::filesystem::is_directory(path))
            m_current_path = path;
        else{
            auto& msgbox = GUI::Application::the().open_overlay<GUI::MessageBox>(path + ": Directory not found!", "Error!", GUI::MessageBox::Buttons::Ok);
            msgbox.exec();
            path_textbox->set_content(m_current_path.string(), NotifyUser::No);
        }
        m_model->update_content(m_current_path);
    };

    parent_directory_button->on_click = [&, path_textbox]()mutable{
        m_current_path = m_current_path.parent_path();
        path_textbox->set_content(m_current_path.string(), NotifyUser::No);
        m_model->update_content(m_current_path);
    };
}

}
