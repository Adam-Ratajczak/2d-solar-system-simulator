#pragma once
#include "../SimulationView.hpp"
#include "../gui/Container.hpp"

class EssaSettings : public GUI::Container {
public:
    explicit EssaSettings(GUI::Container& container, SimulationView& simulation_view);

    bool pause_simulation_on_creative_mode()const {return m_pause_simulation_on_creative_mode;}
    bool unfocus_on_wnd_close()const {return m_unfocus_on_wnd_close;}

private:
    SimulationView& m_simulation_view;
    std::vector<std::function<void()>> m_on_restore_defaults;

    bool m_pause_simulation_on_creative_mode = true;
    bool m_unfocus_on_wnd_close = false;
};
