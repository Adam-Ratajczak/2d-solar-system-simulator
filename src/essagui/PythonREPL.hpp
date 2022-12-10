#pragma once

#ifdef ENABLE_PYSSA

#    include <Essa/GUI/Widgets/ArrowButton.hpp>
#    include <Essa/GUI/Widgets/Console.hpp>
#    include <Essa/GUI/Widgets/Container.hpp>
#    include <Essa/GUI/Widgets/Textbox.hpp>
#    include <Essa/GUI/Widgets/Widget.hpp>

#    include <deque>
#    include <vector>

class PythonREPL : public GUI::Container {
public:
    virtual void on_init() override;

private:
    virtual void handle_event(GUI::Event&) override;
    GUI::Console* m_console {};
    GUI::Textbox* m_textbox {};

    std::vector<Util::UString> m_commands;
    unsigned m_curr_command = 0;
};

#endif
