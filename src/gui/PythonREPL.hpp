#pragma once

#include "ArrowButton.hpp"
#include "Console.hpp"
#include "Container.hpp"
#include "Textbox.hpp"
#include "Widget.hpp"

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <deque>
#include <vector>

namespace GUI {

class PythonREPL : public Container {
public:
    explicit PythonREPL(WidgetTreeRoot& c);

private:
    virtual void handle_event(Event&) override;
    Console* m_console {};
    ArrowButton* m_hide_show_button {};
    Textbox* m_textbox {};

    std::vector<std::string> m_commands;
    unsigned m_curr_command = 0;
};

}
