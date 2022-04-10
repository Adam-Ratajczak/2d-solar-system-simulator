#pragma once

#include "Console.hpp"
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
    Console* m_console {};
    Textbox* m_textbox {};
};
