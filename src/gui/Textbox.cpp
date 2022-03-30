#include "Textbox.hpp"
#include "../World.hpp"
#include "GUI.hpp"
#include "NotifyUser.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cctype>
#include <iostream>

void Textbox::set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color) {
    m_bg_color = bg_color;
    m_fg_color = fg_color;
    m_text_color = text_color;
}

void Textbox::set_content(sf::String content, NotifyUser notify_user) {
    m_content = std::move(content);
    m_cursor = m_content.getSize();
    m_has_decimal = false;

    if (m_content.find(".") != sf::String::InvalidPos)
        m_has_decimal = true;
    if (on_change && notify_user == NotifyUser::Yes)
        on_change(m_content);
}

void Textbox::handle_event(Event& event) {
    auto pos = sf::Mouse::getPosition();
    if (event.type() == sf::Event::TextEntered) {
        if (is_focused()) {
            auto codepoint = event.event().text.unicode;
            if (codepoint == '\b') {
                if (m_cursor != 0) {
                    m_content.erase(m_cursor - 1);
                    m_cursor--;
                    if (on_change)
                        on_change(m_content);
                }
            }
            else if (codepoint == 0x7f) {
                if (m_cursor != m_content.getSize()) {
                    m_content.erase(m_cursor);
                    if (on_change)
                        on_change(m_content);
                }
            }
            else if (can_insert_character(codepoint)) {
                if (m_content == "0" && m_type == NUMBER)
                    set_content("");
                m_content.insert(m_cursor, codepoint);
                if (on_change)
                    on_change(m_content);
                m_cursor++;
            }

            if (m_content.isEmpty() && m_type == NUMBER)
                set_content("0");

            event.set_handled();
        }
    }
    else if (event.type() == sf::Event::KeyPressed) {
        if (is_focused()) {
            // FIXME: Focus check should be handled at Widget level.
            // std::cout << m_cursor << std::endl;
            switch (event.event().key.code) {
            case sf::Keyboard::Left:
                if (m_cursor > 0)
                    m_cursor--;
                break;
            case sf::Keyboard::Right:
                if (m_cursor < m_content.getSize())
                    m_cursor++;
                break;
            default:
                break;
            }
            event.set_handled();
        }
    }
}

void Textbox::draw(sf::RenderWindow& window) const {
    sf::RectangleShape rect(size());
    rect.setFillColor(m_bg_color);

    if (is_focused()) {
        rect.setOutlineColor(m_fg_color);
        rect.setOutlineThickness(3);
    }

    window.draw(rect);

    sf::Text text(m_content, GUI::font, size().y - 4);
    text.setFillColor(m_text_color);
    text.setPosition(2, 2);

    window.draw(text);

    if (is_focused()) {
        sf::RectangleShape cursor(sf::Vector2f(2, 30));
        auto position = text.findCharacterPos(m_cursor);
        cursor.setPosition({ position.x, position.y + size().y / 2 - 15 });
        cursor.setFillColor(sf::Color::Black);
        window.draw(cursor);
    }
}

bool Textbox::can_insert_character(uint32_t ch) const {
    switch (m_type) {
    case TEXT:
        return isprint(ch);
    case NUMBER:
        return isdigit(ch) || (ch == '.' && !m_has_decimal);
    }
    return false;
}
