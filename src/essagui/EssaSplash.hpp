#pragma once

#include "EssaSettings.hpp"
#include <EssaGUI/gui/ToolWindow.hpp>

class EssaSplash : public GUI::ToolWindow {
public:
    explicit EssaSplash(GUI::HostWindow&, EssaSettings& essa_settings);

private:
    virtual void handle_event(llgl::Event) override;

    EssaSettings& m_essa_settings;
};
