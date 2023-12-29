#pragma once

#include "EssaSettings.hpp"
#include <Essa/GUI/Overlays/ToolWindow.hpp>

class EssaSplash : public GUI::WindowRoot {
public:
    explicit EssaSplash(GUI::WidgetTreeRoot&, EssaSettings& essa_settings);

private:
    virtual GUI::Widget::EventHandlerResult handle_event(llgl::Event const&) override;

    EssaSettings& m_essa_settings;
};
