#include "PythonREPL.hpp"

#include "../pyssa/Environment.hpp"
#include "GUI.hpp"
#include "pythonrun.h"

PythonREPL::PythonREPL(Container& c)
    : Container(c) {
    set_layout<BasicLayout>();
    m_textbox = add_widget<Textbox>().get();
    m_textbox->set_size({ { 100, Length::Percent }, 40.0_px });
    m_textbox->set_position({ 0.0_px, 0.0_px_o });
    m_textbox->set_data_type(Textbox::Type::TEXT);
    m_textbox->set_placeholder("PySSA Command");
}

void PythonREPL::handle_event(Event& event) {
    switch (event.type()) {
    case sf::Event::KeyPressed:
        if (!m_textbox->is_focused())
            break;
        if (event.event().key.code == sf::Keyboard::Enter) {
            auto content = m_textbox->get_content().toAnsiString();
            content += "\n";
            append_log_line({ .color = sf::Color(100, 255, 255), .text = ">>> " + content });
            auto result = PySSA::Environment::the().eval_string(content);
            if (!result) {
                std::cout << "ERROR!!" << std::endl;

                auto message = PySSA::Environment::the().generate_exception_message();
                for (auto& line : message)
                    append_log_line({ .color = sf::Color(255, 100, 100), .text = line });
            }
            else {
                append_log_line({ .color = sf::Color(200, 200, 200), .text = result.repr() });
            }
            m_textbox->set_content("");
            event.set_handled();
        }
        break;
    case sf::Event::MouseWheelScrolled: {
        if (!is_hover() || event.event().mouseWheelScroll.wheel != sf::Mouse::VerticalWheel)
            break;
        event.set_handled();
        if (content_size() < scroll_area_size())
            break;
        m_scroll -= event.event().mouseWheelScroll.delta * 15;
        if (m_scroll < 0)
            m_scroll = 0;
        double bottom_content = m_log_lines.size() * 15 - scroll_area_size();
        if (m_scroll > bottom_content)
            m_scroll = bottom_content;
    } break;
    default:
        break;
    }
}

void PythonREPL::append_log_line(LogLine line) {
    m_log_lines.push_back(line);
    double bottom_content = m_log_lines.size() * 15 - scroll_area_size();
    if (bottom_content > 0)
        m_scroll = bottom_content;
}

void PythonREPL::draw(sf::RenderWindow& window) const {
    size_t s = 0;

    for (auto& line : m_log_lines) {
        sf::Text text(line.text, GUI::fixed_width_font, 15);
        text.setPosition(0, s * 15 - m_scroll);
        text.setFillColor(line.color);
        window.draw(text);
        s++;
    }

    // "Scrollbar"
    float scroll_area_size = this->scroll_area_size();
    float content_size = this->content_size();
    if (content_size > scroll_area_size) {
        sf::RectangleShape scrollbar(sf::Vector2f(3, scroll_area_size / content_size * scroll_area_size));
        scrollbar.setPosition(size().x - 3, m_scroll * scroll_area_size / content_size);
        window.draw(scrollbar);
    }
}

float PythonREPL::content_size() const {
    return m_log_lines.size() * 15;
}

float PythonREPL::scroll_area_size() const {
    return size().y - m_textbox->size().y - 15;
}
