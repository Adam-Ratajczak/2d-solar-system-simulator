#pragma once

#include "Container.hpp"
#include "TextButton.hpp"

namespace GUI {

class TabWidget;

class TabSelectWidget : public Container {
public:
    explicit TabSelectWidget(Container& c);

    void add_button(std::string caption, size_t tab_index);
    void switch_to_tab(size_t index);

private:
    std::vector<TextButton*> m_buttons;
};

class TabWidget : public Container {
public:
    explicit TabWidget(Container& c);

    template<class... Args>
    Container& add_tab(std::string caption, Args&&... args) {
        auto tab = m_tab_container->add_widget<Container>(std::forward<Args>(args)...);
        setup_tab(caption, tab.get());
        return *tab.get();
    }

    void switch_to_tab(size_t index);

private:
    void setup_tab(std::string caption, Container* tab);

    TabSelectWidget* m_tab_select {};
    Container* m_tab_container {};
    std::vector<Container*> m_tabs;
};

}
