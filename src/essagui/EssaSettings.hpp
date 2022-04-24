#pragma once
#include "../SimulationView.hpp"
#include "../gui/Container.hpp"

class EssaSettings : public GUI::Container {
public:
    explicit EssaSettings(GUI::Container& container, SimulationView& simulation_view);

private:
    SimulationView& m_simulation_view;
    std::vector<std::function<void()>> m_on_restore_defaults;
};
