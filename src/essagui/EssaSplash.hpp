#pragma once

#include "EssaSettings.hpp"
#include <Essa/GUI/Overlays/ToolWindow.hpp>

class EssaSplash : public GUI::ToolWindow {
public:
    explicit EssaSplash(GUI::MDI::Host&, EssaSettings& essa_settings);

private:
    virtual void handle_event(GUI::Event const&) override;

    EssaSettings& m_essa_settings;
};
