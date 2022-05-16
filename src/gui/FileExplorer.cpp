#include "FileExplorer.hpp"
#include "Container.hpp"
#include "Application.hpp"
#include "MessageBox.hpp"
#include "NotifyUser.hpp"
#include "TextButton.hpp"
#include "Textbox.hpp"
#include "ListView.hpp"
#include "Textfield.hpp"
#include <SFML/Graphics/Color.hpp>
#include <filesystem>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <fstream>
#include <vector>

namespace GUI {

FileExplorer::FileExplorer(Container& c) : Container(c){
    set_layout<VerticalBoxLayout>();
    m_current_path = std::filesystem::absolute("../worlds");

    auto create_prompt_window = [&](const std::string title, const std::string text, std::function<void(std::string)> action){
        auto& prompt_window = GUI::Application::the().open_overlay<GUI::ToolWindow>();
        prompt_window.set_title(title);
        prompt_window.set_size({ 500, 100 });
        prompt_window.center_on_screen();

        auto& container = prompt_window.set_main_widget<GUI::Container>();
        auto& container_layout = container.set_layout<GUI::VerticalBoxLayout>();
        container_layout.set_padding(20);
        container_layout.set_spacing(10);

        auto input_container = container.add_widget<GUI::Container>();
        input_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(20);
        input_container->set_size({ Length::Auto, 30.0_px });

        {
            auto label = input_container->add_widget<GUI::Textfield>();
            label->set_size({ 150.0_px, Length::Auto });
            label->set_content(text);
        }
        auto input = input_container->add_widget<GUI::Textbox>();
        // FIXME: Why textboxes are numeric by default
        input->set_data_type(GUI::Textbox::TEXT);
        input->set_placeholder("e.g solar.essa");

        // FIXME: Also, why buttons are red by default?
        auto button_container = container.add_widget<GUI::Container>();
        button_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(20);
        button_container->set_size({ Length::Auto, 30.0_px });
        {
            auto cancel_button = button_container->add_widget<GUI::TextButton>();
            cancel_button->set_alignment(GUI::Align::Center);
            cancel_button->set_content("Cancel");
            cancel_button->on_click = [prompt_window = &prompt_window]() {
                prompt_window->close();
            };

            auto ok_button = button_container->add_widget<GUI::TextButton>();
            ok_button->set_alignment(GUI::Align::Center);
            ok_button->set_content("OK");
            ok_button->on_click = [&](){
                action(input->get_content());
                prompt_window.close();
            };
        }

        prompt_window.run();

        return prompt_window;
    };

    auto toolbar = add_widget<Container>();
    toolbar->set_layout<HorizontalBoxLayout>();
    toolbar->set_size({Length::Auto, 30.0_px});

    auto path_textbox = toolbar->add_widget<Textbox>();
    path_textbox->set_data_type(Textbox::Type::TEXT);
    path_textbox->set_content(m_current_path.string());
    path_textbox->set_size({{50.0, Length::Percent}, Length::Auto});

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
    create_directory_button->on_click = [&](){
        create_prompt_window("Create folder", "Folder name: ", [&](std::string path){
            std::filesystem::create_directory(m_current_path.string() + "\\" + path);
            m_model->update_content(m_current_path);
        });
    };

    auto create_file_button = toolbar->add_widget<TextButton>();
    create_file_button->set_content("Create file");
    create_file_button->set_tooltip_text("Create file");
    create_file_button->set_alignment(Align::Center);
    create_file_button->set_background_color(sf::Color::Blue);
    create_file_button->on_click = [&](){
        create_prompt_window("Create file", "File name with extension: ", [&](std::string path){
            std::ofstream f_out(m_current_path.string() + "\\" + path);
            f_out.close();
            m_model->update_content(m_current_path);
        });
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

    list->on_click = [&, path_textbox](unsigned row){
        auto path = m_model->get_path(row);
        if(std::filesystem::is_directory(path))
            m_current_path = path;
        m_model->update_content(m_current_path);
        path_textbox->set_content(m_current_path.string(), NotifyUser::No);
    };

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
