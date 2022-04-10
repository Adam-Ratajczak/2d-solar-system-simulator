#include "TextButton.hpp"
#include "Button.hpp"
#include "GUI.hpp"
#include <SFML/Window/Event.hpp>

void TextButton::handle_event(Event& event){
    if(is_hover()){
        if(event.type() == sf::Event::MouseButtonPressed){
            m_active = !m_active;
            on_change(m_active);
        }
    }
}

void TextButton::draw(sf::RenderWindow& window) const {
    sf::RectangleShape rect(size());
    sf::Text text(m_content, GUI::font, 15);

    if(!m_active){
        rect.setFillColor(m_bg_color);
        rect.setOutlineColor(m_fg_color);
        rect.setOutlineThickness(3);
        text.setFillColor(m_text_color);
    }else{
        rect.setFillColor(m_active_bg_color);
        rect.setOutlineColor(m_active_fg_color);
        rect.setOutlineThickness(3);
        text.setFillColor(m_active_text_color);
        text.setString(m_active_content);
    }

    window.draw(rect);

    align_text(m_alignment, size(), text);

    window.draw(text);
}

void TextButton::set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color){
    m_bg_color = bg_color;
    m_fg_color = fg_color;
    m_text_color = text_color;
}

void TextButton::set_active_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color){
    m_active_bg_color = bg_color;
    m_active_fg_color = fg_color;
    m_active_text_color = text_color;
}
