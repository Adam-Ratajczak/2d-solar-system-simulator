#pragma once

#include "Button.hpp"
#include "Slider.hpp"
#include "Textbox.hpp"
#include "Widget.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <random>
#include <type_traits>

class GUI
{
    sf::RenderWindow& m_window;

    std::shared_ptr<Button> m_create_button;
    std::shared_ptr<Slider> m_slider_1;
    std::shared_ptr<Slider> m_slider_2;
    std::shared_ptr<Slider> m_slider_3;
    std::shared_ptr<Textbox> m_text_1;

    std::vector<std::shared_ptr<Widget>> m_widgets;

public:
    GUI(sf::RenderWindow& window);
    void draw();
    void get_events(sf::Event& event);

    template<class T, class... Args> requires(std::is_base_of_v<Widget, T> && requires(GUI gui, Args&&... args) { T(gui, args...); } )
    std::shared_ptr<T> add_widget(Args&&... args)
    {
        auto widget = std::make_shared<T>(*this, std::forward<Args>(args)...);
        m_widgets.push_back(widget);
        return widget;
    }
};
