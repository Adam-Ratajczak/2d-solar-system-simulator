#pragma once

#include "EssaSettings.hpp"
#include <EssaGUI/gui/ToolWindow.hpp>

class EssaSplash : public GUI::ToolWindow {
public:
    explicit EssaSplash(sf::RenderWindow& wnd, EssaSettings& essa_settings);

private:
    virtual void handle_event(sf::Event) override;

    EssaSettings& m_essa_settings;
};
