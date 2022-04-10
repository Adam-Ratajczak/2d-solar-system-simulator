#pragma once

#include "Container.hpp"
#include "Textbox.hpp"

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <deque>

class PythonREPL : public Container {
public:
    explicit PythonREPL(Container& c);

private:
    virtual void handle_event(Event&) override;
    virtual void draw(sf::RenderWindow&) const override;

    struct LogLine {
        sf::Color color;
        std::string text;
    };
    void append_log_line(LogLine);

    std::deque<LogLine> m_log_lines;
    Textbox* m_textbox {};
    float m_scroll {};

    float content_size() const;
    float scroll_area_size() const;
};
