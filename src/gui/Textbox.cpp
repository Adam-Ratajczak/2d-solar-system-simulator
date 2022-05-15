#include "Textbox.hpp"

#include "../util/CharacterType.hpp"
#include "Application.hpp"
#include "NotifyUser.hpp"
#include "Widget.hpp"
#include <SFML/Graphics.hpp>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

namespace GUI {

void Textbox::set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color) {
    m_bg_color = bg_color;
    m_fg_color = fg_color;
    m_text_color = text_color;
}

unsigned Textbox::m_character_pos_from_mouse(Event& event) {
    if (m_content.getSize() == 0 || (m_content == "0" && m_type == Type::NUMBER))
        return 0;
    auto delta = event.mouse_position() - position();
    if (delta.x < 0)
        return 0;

    sf::Text text = generate_sf_text();

    // We can just check the offset of 1st character because we use
    // a fixed width font. Normally we would need to iterate over characters
    // to find the nearest one.
    float character_width = text.findCharacterPos(1).x - text.getPosition().x;
    auto cursor = (delta.x - m_scroll) / character_width;
    return std::min(m_content.getSize(), (size_t)cursor);
}

void Textbox::set_content(sf::String content, NotifyUser notify_user) {
    m_content = std::move(content);
    m_cursor = m_content.getSize();
    m_selection_start = m_cursor;
    m_has_decimal = false;

    if (m_content.find(".") != sf::String::InvalidPos)
        m_has_decimal = true;
    if (on_change && notify_user == NotifyUser::Yes)
        on_change(m_content);
}

void Textbox::interactive_set_cursor(unsigned cursor, ExtendSelection extend_selection) {
    if (cursor > m_content.getSize())
        m_cursor = m_content.getSize();
    else
        m_cursor = cursor;

    if (extend_selection == ExtendSelection::No || !m_shift_pressed)
        m_selection_start = m_cursor;

    auto new_cursor_position = calculate_cursor_position();
    if (new_cursor_position.x < 2)
        m_scroll += 2 - new_cursor_position.x;
    if (new_cursor_position.x > size().x - 4)
        m_scroll += size().x - new_cursor_position.x - 4;
}

bool Textbox::find_decimal() const {
    for (const auto& c : m_content) {
        if (c == '.')
            return true;
    }
    return false;
}

void Textbox::handle_event(Event& event) {
    auto pos = sf::Mouse::getPosition();
    if (event.type() == sf::Event::TextEntered) {
        if (is_focused()) {
            auto codepoint = event.event().text.unicode;
            if (codepoint == '\b') {
                if (m_cursor == m_selection_start) {
                    if (m_cursor != 0) {
                        m_content.erase(m_cursor - 1);
                        interactive_set_cursor(m_cursor - 1, ExtendSelection::No);
                    }
                }
                else
                    erase_selected_text();
                if (m_type == NUMBER && m_content.isEmpty())
                    m_content = "0";
                if (on_change)
                    on_change(m_content);

                m_has_decimal = find_decimal();
            }
            else if (codepoint == 0x7f) {
                if (m_cursor != m_content.getSize()) {
                    erase_selected_text();
                    if (m_type == NUMBER && m_content.isEmpty())
                        m_content = "0";
                    if (on_change)
                        on_change(m_content);
                }
            }
            else if (can_insert_character(codepoint)) {
                if (m_type == NUMBER && m_content == "0")
                    m_content = "";
                if (m_cursor != m_selection_start)
                    erase_selected_text();
                m_content.insert(m_cursor, codepoint);
                if (on_change)
                    on_change(m_content);
                interactive_set_cursor(m_cursor + 1, ExtendSelection::No);
            }

            event.set_handled();
        }
    }
    else if (event.type() == sf::Event::KeyPressed) {
        m_shift_pressed = event.event().key.shift;
        if (is_focused()) {
            // FIXME: Focus check should be handled at Widget level.
            switch (event.event().key.code) {
            case sf::Keyboard::Left: {
                if (event.event().key.control)
                    move_cursor_by_word(CursorDirection::Left);
                else if (m_cursor > 0)
                    interactive_set_cursor(m_cursor - 1);
                else if (!m_shift_pressed) {
                    if (m_cursor < m_selection_start)
                        m_selection_start = m_cursor;
                    else
                        m_cursor = m_selection_start;
                }

                event.set_handled();
            } break;
            case sf::Keyboard::Right: {
                if (event.event().key.control)
                    move_cursor_by_word(CursorDirection::Right);
                else if (m_cursor < m_content.getSize())
                    interactive_set_cursor(m_cursor + 1);
                else if (!m_shift_pressed) {
                    if (m_cursor > m_selection_start)
                        m_selection_start = m_cursor;
                    else
                        m_cursor = m_selection_start;
                }

                event.set_handled();
            } break;
            case sf::Keyboard::A: {
                if (event.event().key.control) {
                    m_cursor = m_content.getSize();
                    m_selection_start = 0;
                }
                break;
            }
            case sf::Keyboard::C: {
                if (event.event().key.control) {
                    sf::Clipboard::setString(selected_text());
                }
                break;
            }
            case sf::Keyboard::V: {
                if (event.event().key.control) {
                    m_selection_start = m_cursor;
                    auto clipboard_contents = sf::Clipboard::getString();
                    m_content.insert(m_cursor, clipboard_contents);
                    m_cursor += clipboard_contents.getSize();
                }
                break;
            }
            default:
                break;
            }
        }
    }
    else if (event.type() == sf::Event::MouseButtonPressed) {
        if (is_hover()) {
            interactive_set_cursor(m_character_pos_from_mouse(event));
            m_selection_start = m_cursor;
            m_dragging = true;
        }
    }
    else if (event.type() == sf::Event::MouseButtonReleased) {
        m_dragging = false;
    }
    else if (event.type() == sf::Event::MouseMoved) {
        if (m_dragging)
            m_cursor = m_character_pos_from_mouse(event);
    }

    if (m_type == NUMBER && m_has_limit)
        m_fit_in_range();
}

sf::String Textbox::selected_text() const {
    auto start = std::min(m_cursor, m_selection_start);
    auto size = std::max(m_cursor, m_selection_start) - start;
    return m_content.substring(start, size);
}

void Textbox::erase_selected_text() {
    auto start = std::min(m_cursor, m_selection_start);
    m_content.erase(start, std::max(m_cursor, m_selection_start) - start);
    m_cursor = start;
    m_selection_start = m_cursor;
}

std::string Textbox::m_fix_content(std::string content) const {
    unsigned i = content.size();
    while (i > 0 && content[i - 1] == '0')
        i--;

    if (content[i - 1] == '.')
        return content.substr(0, i) + '0';
    return content.substr(0, std::min(m_limit, i));
}

void Textbox::m_fit_in_range() {
    if (is_focused())
        return;

    try {
        double val = std::stod(m_content.toAnsiString());
        std::ostringstream oss;
        oss << std::fixed;
        if (val < m_min_value)
            oss << m_min_value;
        else if (val > m_max_value)
            oss << m_max_value;
        else
            return;
        m_content = m_fix_content(oss.str());
    } catch (...) {
        return;
    }
}

void Textbox::move_cursor_by_word(CursorDirection direction) {
    // Trying to mimic vscode behavior in c++ source files.
    enum class State {
        Start,
        PendingPunctuation,
        PendingCharactersOfType,
        Done
    } state
        = State::Start;
    Util::CharacterType last_character_type = Util::CharacterType::Unknown;

    auto is_in_range = [&](int offset) {
        return (direction == CursorDirection::Left && offset > 0) || (direction == CursorDirection::Right && offset < m_content.getSize());
    };

    auto new_cursor = m_cursor;
    while (state != State::Done && is_in_range(new_cursor)) {
        auto next = m_content[direction == CursorDirection::Left ? new_cursor - 1 : new_cursor + 1];
        // std::cout << "'" << (char)next << "' " << (int)state << " : " << ispunct(next) << std::endl;
        switch (state) {
        case State::Start:
            if (ispunct(next))
                state = State::PendingCharactersOfType;
            else if (!isspace(next)) {
                if (is_in_range(new_cursor - 2) && ispunct(m_content[direction == CursorDirection::Left ? new_cursor - 1 : new_cursor + 1]))
                    state = State::PendingPunctuation;
                else
                    state = State::PendingCharactersOfType;
                new_cursor++;
            }
            break;
        case State::PendingPunctuation:
            assert(ispunct(next));
            state = State::PendingCharactersOfType;
            break;
        case State::PendingCharactersOfType: {
            auto next_type = Util::character_type(next);
            if (next_type != last_character_type && last_character_type != Util::CharacterType::Unknown) {
                state = State::Done;
                if (direction == CursorDirection::Left)
                    new_cursor++;
            }
            last_character_type = next_type;
            break;
        }
        default:
            assert(false);
            break;
        }
        if (direction == CursorDirection::Left)
            new_cursor--;
        else if (new_cursor < m_content.getSize())
            new_cursor++;
    }
    interactive_set_cursor(new_cursor);
}

sf::Text Textbox::generate_sf_text() const {
    // TODO: Cache the result
    sf::Text text(m_content, Application::the().fixed_width_font, 16);
    text.setFillColor(m_text_color);
    text.setPosition(5, 2 + size().y / 2 - 12);
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
    rect.setOutlineColor(sf::Color(80, 80, 80));
    rect.setOutlineThickness(-2);
    rect.setFillColor(are_all_parents_enabled() ? m_bg_color : m_bg_color - sf::Color(60, 60, 60, 0));

    if (is_focused())
        rect.setOutlineColor(are_all_parents_enabled() ? m_fg_color : m_fg_color - sf::Color(39, 39, 39, 0));

    window.draw(rect);

    auto const cursor_height = std::min(size().y - 6, 30.f);
    auto text = generate_sf_text();
    auto text_bounds = text.getGlobalBounds();
    auto selection_start_pos = text.findCharacterPos(std::min(m_selection_start, m_cursor)).x;
    auto selection_end_pos = text.findCharacterPos(std::max(m_selection_start, m_cursor)).x;
    sf::RectangleShape selected_rect({ selection_end_pos - selection_start_pos, cursor_height });
    selected_rect.setFillColor(is_focused() ? sf::Color(160, 160, 255) : sf::Color(160, 160, 160));
    selected_rect.setPosition({ selection_start_pos, size().y / 2 - selected_rect.getSize().y / 2 });
    window.draw(selected_rect);
    window.draw(text);

    if (is_focused()) {
        // std::cout << m_cursor << std::endl;
        if ((m_cursor_clock.getElapsedTime().asMilliseconds() / 500) % 2 == 0)
            return;

        sf::RectangleShape cursor(sf::Vector2f(2, cursor_height));
        auto position = calculate_cursor_position();
        cursor.setPosition({ position.x, size().y / 2 - cursor.getSize().y / 2 });
        cursor.setFillColor(sf::Color::Black);
        window.draw(cursor);
    }
}

bool Textbox::can_insert_character(uint32_t ch) const {
    if (m_content.getSize() >= m_limit)
        return false;

    switch (m_type) {
    case TEXT:
        return isprint(ch);
    case NUMBER:
        return isdigit(ch) || (ch == '.' && !m_has_decimal);
    }
    return false;
}
}
