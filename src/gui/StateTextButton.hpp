#pragma once
#include "TextAlign.hpp"
#include "Widget.hpp"
#include "Application.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <vector>
#include <functional>

namespace GUI {

template <typename T>
class StateTextButton : public Widget {
    struct State{
        sf::Color bg_color, fg_color, text_color;
        T state;
    };
public:
    explicit StateTextButton(Container& c);

    void add_state(sf::String content, T state, sf::Color bg_color, sf::Color fg_color = sf::Color::Blue, sf::Color text_color = sf::Color::White);

    unsigned get_font_size() const { return m_font_size; }
    void set_font_size(unsigned font_size) { m_font_size = font_size; }

    Align get_alignment() const { return m_alignment; };
    void set_alignment(Align alignment) { m_alignment = alignment; };

    std::pair<std::string, State> get_element_from_index(unsigned index) const;

    T state() const;

    std::function<void(T)> on_change;

    void set_bg_color(sf::Color color);
    void set_fg_color(sf::Color color);
    void set_text_color(sf::Color color);

    void set_index(unsigned index);

private:
    virtual void draw(sf::RenderWindow& window) const override;
    virtual void handle_event(Event& event) override;

    std::map<std::string, State> m_states;

    unsigned m_font_size = 20, m_index = 0;
    Align m_alignment = Align::CenterLeft;
};

// TODO: move to source code file

template <typename T>
StateTextButton<T>::StateTextButton(Container& c)
    : Widget(c) {}

template <typename T>
std::pair<std::string, typename StateTextButton<T>::State> StateTextButton<T>::get_element_from_index(unsigned index) const{
    unsigned i = 0;

    for(const auto& p : m_states){
        if(i == index)
            return p;
        i++;
    }
    return {};
}

template <typename T>
void StateTextButton<T>::add_state(sf::String content, T state, sf::Color bg_color, sf::Color fg_color, sf::Color text_color){
    m_states[content] = {bg_color, fg_color, text_color, state};
;
}

template <typename T>
void StateTextButton<T>::set_index(unsigned int index){
    if(index >= m_states.size())
        m_index = m_states.size() - 1;
    else
        m_index = index;
    on_change(state());
}

template <typename T>
void StateTextButton<T>::handle_event(Event& event){
    if(event.type() == sf::Event::MouseButtonPressed){
        if(is_hover()){
            m_index++;
            if(m_index >= m_states.size())
                m_index = 0;
            
            on_change(state());
        }
    }
}

template <typename T>
void StateTextButton<T>::draw(sf::RenderWindow& window) const {
    sf::RectangleShape rect(size());
    rect.setFillColor(get_element_from_index(m_index).second.bg_color);
    rect.setOutlineColor(get_element_from_index(m_index).second.fg_color);
    rect.setOutlineThickness(3);
    window.draw(rect);

    sf::Text text(get_element_from_index(m_index).first, this->application().font, 15);
    text.setFillColor(get_element_from_index(m_index).second.text_color);
    align_text(m_alignment, size(), text);

    window.draw(text);
}

template <typename T>
T StateTextButton<T>::state() const{
    return get_element_from_index(m_index).second.state;
}

template <typename T>
void StateTextButton<T>::set_bg_color(sf::Color color){
    for(auto& s : m_states)
        s.bg_color = color;
}

template <typename T>
void StateTextButton<T>::set_fg_color(sf::Color color){
    for(auto& s : m_states)
        s.fg_color = color;
}

template <typename T>
void StateTextButton<T>::set_text_color(sf::Color color){
    for(auto& s : m_states)
        s.text_color = color;
}

}
