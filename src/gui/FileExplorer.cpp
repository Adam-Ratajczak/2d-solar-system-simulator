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

class FileView : public Model {
public:
    virtual size_t row_count() const override {
        return m_content.size();
    }
    virtual size_t column_count() const override {
        return 4;
    }
    virtual std::string data(size_t row, size_t column) const override {
        return m_content[row][column];
    }
    virtual Column column(size_t column) const override {
        switch(column) {
            case 0: return {.width = 200, .name = "Name"};
            case 1: return {.width = 100, .name = "Size"};
            case 2: return {.width = 200, .name=  "Modify date"};
            case 3: return {.width = 100, .name=  "File type"};
        }
        return {};
    }

    void update_content(std::filesystem::path path){
        m_content.clear();
        for(const auto& o : std::filesystem::directory_iterator(path)){
            std::time_t cftime = std::chrono::system_clock::to_time_t(
            std::chrono::file_clock::to_sys(o.last_write_time()));

            unsigned size = 0;

            if(std::filesystem::is_directory(o)){
                for(const auto& d : std::filesystem::recursive_directory_iterator(o)){
                    if(!std::filesystem::is_directory(d))
                        size += d.file_size();
                }
            }else
                size = o.file_size();

            m_content.push_back(std::vector<std::string>(4));
            m_content.back()[0] = o.path().filename();
            m_content.back()[1] = std::to_string(size);
            m_content.back()[2] = std::asctime(std::localtime(&cftime));
            m_content.back()[3] = o.path().extension();

            // for(const auto& e : m_content.back())
            //     std::cout << e << "\t";
            // std::cout << "\n";
        }
    }
private:
    std::vector<std::vector<std::string>> m_content;
};

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

    m_list = main_container->add_widget<ListView>();
    auto model = m_list->create_and_set_model<FileView>();
    model.update_content(m_current_path);

    path_textbox->on_enter = [&, path_textbox, model](std::string path) mutable{
        if(std::filesystem::exists(path) && std::filesystem::is_directory(path))
            m_current_path = path;
        else{
            auto& msgbox = GUI::Application::the().open_overlay<GUI::MessageBox>(path + ": Directory not found!", "Error!", GUI::MessageBox::Buttons::Ok);
            msgbox.exec();
            path_textbox->set_content(m_current_path.string(), NotifyUser::No);
        }
        model.update_content(m_current_path);
    };

    parent_directory_button->on_click = [&, path_textbox, model]()mutable{
        m_current_path = m_current_path.parent_path();
        path_textbox->set_content(m_current_path.string(), NotifyUser::No);
        model.update_content(m_current_path);
    };
}

}
