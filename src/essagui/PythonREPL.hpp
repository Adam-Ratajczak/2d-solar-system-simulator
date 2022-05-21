#pragma once

#include <EssaGUI/gui/ArrowButton.hpp>
#include <EssaGUI/gui/Console.hpp>
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/Textbox.hpp>
#include <EssaGUI/gui/Widget.hpp>

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
