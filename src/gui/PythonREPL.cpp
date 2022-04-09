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
        if (event.event().key.code == sf::Keyboard::Enter) {
            auto content = m_textbox->get_content().toAnsiString();
            content += "\n";
            append_log_line({ .color = sf::Color(100, 255, 255), .text = ">>> " + content });
            auto result = PySSA::Environment::the().eval_string(content);
            if (!result) {
                std::cout << "ERROR!!" << std::endl;

                auto message = PySSA::Environment::the().generate_exception_message();
                for (auto& line : message)
                    append_log_line({ .color = sf::Color(255, 100, 100), .text = line, .time = m_timer.getElapsedTime()});
            }
            else {
                append_log_line({ .color = sf::Color(200, 200, 200), .text = result.repr(), .time = m_timer.getElapsedTime()});
            }
            m_textbox->set_content("");
            event.set_handled();
        }
        break;
    default:
        break;
    }
    if(m_log_lines.size() > 0)
        erase_log_line();
}

void PythonREPL::append_log_line(LogLine line) {
    m_log_lines.push_back(line);
    if (m_log_lines.size() > (size().y - m_textbox->size().y - 15) / 15)
        m_log_lines.pop_front();
}

void PythonREPL::erase_log_line(){
    // std::cout << m_timer.getElapsedTime().asMilliseconds() << "\n";
    if(m_timer.getElapsedTime().asMilliseconds() - m_log_lines.front().time.asMilliseconds() > 6000)
        m_log_lines.pop_front();
}

void PythonREPL::draw(sf::RenderWindow& window) const {
    size_t s = 0;

    for (auto& line : m_log_lines) {
        sf::Text text(line.text, GUI::font, 15);
        text.setPosition(0, s * 15);
        text.setFillColor(line.color);
        window.draw(text);
        s++;
    }
}
