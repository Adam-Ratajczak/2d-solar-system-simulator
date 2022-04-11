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
#include <string>

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

void Textbox::set_cursor(unsigned cursor) {
    auto old_cursor = m_cursor;
    if (cursor > m_content.getSize())
        m_cursor = m_content.getSize();
    else
        m_cursor = cursor;

    auto new_cursor_position = calculate_cursor_position();
    if (new_cursor_position.x < 2)
        m_scroll += 2 - new_cursor_position.x;
    if (new_cursor_position.x > size().x - 4)
        m_scroll += size().x - new_cursor_position.x - 4;
}

void Textbox::handle_event(Event& event) {
    auto pos = sf::Mouse::getPosition();
    if (event.type() == sf::Event::TextEntered) {
        if (is_focused()) {
            auto codepoint = event.event().text.unicode;
            if (codepoint == '\b') {
                if (m_cursor != 0) {
                    m_content.erase(m_cursor - 1);
                    set_cursor(m_cursor - 1);
                    if (m_type == NUMBER && m_content.isEmpty())
                        m_content = "0";
                    if (on_change)
                        on_change(m_content);
                }
            }
            else if (codepoint == 0x7f) {
                if (m_cursor != m_content.getSize()) {
                    m_content.erase(m_cursor);
                    if (m_type == NUMBER && m_content.isEmpty())
                        m_content = "0";
                    if (on_change)
                        on_change(m_content);
                }
            }
            else if (can_insert_character(codepoint)) {
                if (m_type == NUMBER && m_content == "0")
                    m_content = "";
                m_content.insert(m_cursor, codepoint);
                if (on_change)
                    on_change(m_content);
                set_cursor(m_cursor + 1);
            }

            event.set_handled();
        }
    }
    else if (event.type() == sf::Event::KeyPressed) {
        if (is_focused()) {
            // FIXME: Focus check should be handled at Widget level.
            // std::cout << m_cursor << std::endl;
            switch (event.event().key.code) {
            case sf::Keyboard::Left: {
                if (m_cursor > 0)
                    set_cursor(m_cursor - 1);
                event.set_handled();
                break;
            }
            case sf::Keyboard::Right: {
                if (m_cursor < m_content.getSize())
                    set_cursor(m_cursor + 1);
                event.set_handled();
                break;
            }
            default:
                break;
            }
        }
    }
    else if (event.type() == sf::Event::MouseButtonPressed) {
        if (is_hover()) {
            if (m_content.getSize() == 0)
                return;
            auto delta = sf::Vector2f(event.event().mouseButton.x, event.event().mouseButton.y) - position();

            sf::Text text = generate_sf_text();
            
            // We can just check the offset of 1st character because we use
            // a fixed width font. Normally we would need to iterate over characters
            // to find the nearest one.
            float character_width = text.findCharacterPos(1).x - text.getPosition().x;
            m_cursor = (delta.x - m_scroll) / character_width;

            if (delta.x < 0)
                m_cursor = 0;
            else if (m_cursor > m_content.getSize())
                m_cursor = m_content.getSize();
        }
    }
}

sf::Text Textbox::generate_sf_text() const {
    // TODO: Cache the result
    sf::Text text(m_content, GUI::fixed_width_font, 16);
    text.setFillColor(m_text_color);
    text.setPosition(2, 2 + size().y / 2 - 12);
    text.move(m_scroll, 0);
    auto bounds = text.getLocalBounds();
    text.setOrigin(0, 0); // -6 because of SFML not taking descenders into account

    if (!is_focused() && m_content.getSize() == 0) {
        text.setFillColor(m_placeholder_color);
        text.setString(m_placeholder);
    }

    return text;
}

sf::Vector2f Textbox::calculate_cursor_position() const {
    return generate_sf_text().findCharacterPos(m_cursor);
}

void Textbox::draw(sf::RenderWindow& window) const {
    sf::RectangleShape rect(size());
    rect.setFillColor(are_all_parents_enabled() ? m_bg_color : m_bg_color - sf::Color(60, 60, 60, 0));

    if (is_focused()) {
        rect.setOutlineColor(are_all_parents_enabled() ? m_fg_color : m_fg_color - sf::Color(39, 39, 39, 0));
        rect.setOutlineThickness(3);
    }

    window.draw(rect);

    auto text = generate_sf_text();
    window.draw(text);

    if (is_focused()) {
        sf::RectangleShape cursor(sf::Vector2f(2, 30));
        auto position = calculate_cursor_position();
        cursor.setPosition({ position.x, size().y / 2 - 15 });
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
