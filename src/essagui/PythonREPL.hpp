#pragma once

#include "../gui/ArrowButton.hpp"
#include "../gui/Console.hpp"
#include "../gui/Container.hpp"
#include "../gui/Textbox.hpp"
#include "../gui/Widget.hpp"

#include <deque>
#include <vector>

class PythonREPL : public GUI::Container {
public:
    explicit PythonREPL(GUI::WidgetTreeRoot& c);

private:
    virtual void handle_event(GUI::Event&) override;
    GUI::Console* m_console {};
    GUI::Textbox* m_textbox {};

    std::vector<std::string> m_commands;
    unsigned m_curr_command = 0;
};
