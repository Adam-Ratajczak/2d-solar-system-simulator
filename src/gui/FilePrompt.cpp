#include "FilePrompt.hpp"

#include "Application.hpp"
#include "Container.hpp"
#include "TextButton.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include "ToolWindow.hpp"

namespace GUI {

FilePrompt::FilePrompt(sf::RenderWindow& wnd, sf::String help_text, sf::String window_title, sf::String placeholder)
    : ToolWindow(wnd, "Prompt"){

    set_title(std::move(window_title));
    set_size({ 500, 100 });
    center_on_screen();
    // set_id("WND");

    auto& container = set_main_widget<Container>();
    auto& container_layout = container.set_layout<VerticalBoxLayout>();
    container_layout.set_padding(20);
    container_layout.set_spacing(10);

    auto input_container = container.add_widget<Container>();
    input_container->set_layout<HorizontalBoxLayout>().set_spacing(20);
    input_container->set_size({ Length::Auto, 30.0_px });

    {
        auto label = input_container->add_widget<Textfield>();
        label->set_size({ 150.0_px, Length::Auto });
        label->set_content(help_text);
    }
    auto input = input_container->add_widget<Textbox>();
    // FIXME: Why textboxes are numeric by default
    input->set_data_type(Textbox::TEXT);
    input->set_placeholder("e.g solar.essa");

    auto file_btn = input_container->add_widget<TextButton>();
    file_btn->set_content("Browse file");

    file_btn->on_click = [&]() {
        auto& file_explorer_wnd = GUI::Application::the().open_overlay<ToolWindow>();
        file_explorer_wnd.set_size({ 1000, 600 });
        file_explorer_wnd.center_on_screen();

        auto& file_explorer_container = file_explorer_wnd.set_main_widget<Container>();
        file_explorer_container.set_layout<HorizontalBoxLayout>();
        m_file_explorer = file_explorer_container.add_widget<FileExplorer>();
        m_file_explorer->model()->add_desired_extension(".essa");
        
        m_file_explorer->on_submit = [&](std::filesystem::path path)mutable{
            input->set_content(path.relative_path().string());
        
            file_explorer_wnd.close();
        };

        on_close = [&](){
            file_explorer_wnd.close();
        };
    };

    // FIXME: Also, why buttons are red by default?
    auto button_container = container.add_widget<Container>();
    button_container->set_layout<HorizontalBoxLayout>().set_spacing(20);
    button_container->set_size({ Length::Auto, 30.0_px });
    {
        auto cancel_button = button_container->add_widget<TextButton>();
        cancel_button->set_alignment(Align::Center);
        cancel_button->set_content("Cancel");
        cancel_button->on_click = [&]() mutable{
            close();
        };

        auto ok_button = button_container->add_widget<TextButton>();
        ok_button->set_alignment(Align::Center);
        ok_button->set_content("OK");
        ok_button->on_click = [&, input]()mutable {
            m_result = input->get_content();
            close();
        };
    }
};

FilePrompt* file_prompt(sf::String help_text, sf::String window_title, sf::String placeholder) {
    auto& prompt = Application::the().open_overlay<FilePrompt>(std::move(help_text), std::move(window_title), std::move(placeholder));

    prompt.run();

    return &prompt;
}

}
