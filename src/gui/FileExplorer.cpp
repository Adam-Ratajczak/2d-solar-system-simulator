#include "FileExplorer.hpp"
#include "Application.hpp"
#include "Container.hpp"
#include "ListView.hpp"
#include "MessageBox.hpp"
#include "NotifyUser.hpp"
#include "Prompt.hpp"
#include "TextButton.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include <SFML/Graphics/Color.hpp>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace GUI {

FileExplorer::FileExplorer(Container& c)
    : Container(c) {
    set_layout<VerticalBoxLayout>();

    auto toolbar = add_widget<Container>();
    toolbar->set_layout<HorizontalBoxLayout>();
    toolbar->set_size({ Length::Auto, 30.0_px });

    m_path_textbox = toolbar->add_widget<Textbox>();
    m_path_textbox->set_data_type(Textbox::Type::TEXT);
    m_path_textbox->set_content(m_current_path.string());
    m_path_textbox->set_size({ { 50.0, Length::Percent }, Length::Auto });

    auto parent_directory_button = toolbar->add_widget<TextButton>();
    parent_directory_button->set_content("Parent");
    parent_directory_button->set_tooltip_text("Parent");
    parent_directory_button->set_alignment(Align::Center);
    parent_directory_button->set_background_color(sf::Color::Green);

    auto create_directory_button = toolbar->add_widget<TextButton>();
    create_directory_button->set_content("Create folder");
    create_directory_button->set_tooltip_text("Create folder");
    create_directory_button->set_alignment(Align::Center);
    create_directory_button->set_background_color(sf::Color::Blue);
    create_directory_button->on_click = [&]() {
        auto path = GUI::prompt("Folder name: ", "Create folder");
        if (path.has_value()) {
            // C++ Why mutable paths?!!!
            auto new_path = m_current_path;
            new_path.append(path->toAnsiString());
            std::filesystem::create_directory(new_path);
            m_model->update_content(m_current_path);
        };
    };

    auto create_file_button = toolbar->add_widget<TextButton>();
    create_file_button->set_content("Create file");
    create_file_button->set_tooltip_text("Create file");
    create_file_button->set_alignment(Align::Center);
    create_file_button->set_background_color(sf::Color::Blue);
    create_file_button->on_click = [&]() {
        auto file_name = GUI::prompt("File name with extension: ", "Create file");
        if (file_name.has_value()) {
            // C++ Why mutable paths?!!!
            auto new_path = m_current_path;
            new_path.append(file_name->toAnsiString());
            std::ofstream f_out(new_path);
            m_model->update_content(m_current_path);
        };
    };

    auto find = toolbar->add_widget<Textbox>();
    find->set_placeholder("Find file or directory");
    find->set_size({ { 25.0, Length::Percent }, Length::Auto });
    find->set_data_type(Textbox::Type::TEXT);
    find->on_change = [&](std::string content){
        m_model->update_content(m_current_path, [&](std::filesystem::path path){return path.string().substr(0, content.size()) == content;});
    };

    auto main_container = add_widget<Container>();
    main_container->set_layout<HorizontalBoxLayout>().set_spacing(10);

    auto sidebar = main_container->add_widget<Container>();
    sidebar->set_size({ { 20.0, Length::Percent }, Length::Auto });

    auto list = main_container->add_widget<ListView>();
    m_model = &list->create_and_set_model<FileView>();

    list->on_click = [&](unsigned row) {
        open_path(m_model->get_path(row));
    };

    m_path_textbox->on_enter = [&](std::string path) {
        open_path(path);
    };

    parent_directory_button->on_click = [&]() {
        m_current_path = m_current_path.parent_path();
        m_path_textbox->set_content(m_current_path.string(), NotifyUser::No);
        m_model->update_content(m_current_path);
    };

    open_path("../worlds");
}

void FileExplorer::open_path(std::filesystem::path path) {
    path = std::filesystem::absolute(path).lexically_normal();
    try {
        m_model->update_content(path);
    } catch (std::filesystem::filesystem_error& error) {
        m_model->update_content(m_current_path);
        GUI::message_box(error.path1().string() + ": " + error.code().message(), "Error!", GUI::MessageBox::Buttons::Ok);
        return;
    }
    m_current_path = path;
    m_path_textbox->set_content(path.string(), NotifyUser::No);
}

}
