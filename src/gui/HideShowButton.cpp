#include "HideShowButton.hpp"

namespace GUI {

HideShowButton::HideShowButton(Container& c)
    : Button(c) {
    set_toggleable(true);
}

void HideShowButton::draw(sf::RenderWindow& window) const {
    sf::RectangleShape background(size());
    background.setFillColor(bg_color_for_state());
    window.draw(background);

    constexpr float ArrowSize = 10;
    sf::Color const Color { 200, 200, 200 };

    sf::VertexArray arrow(sf::Triangles, 3);
    auto active_factor = (is_active() ? ArrowSize : -ArrowSize) / 4;
    arrow[0] = { { std::round(size().x / 2 - ArrowSize / 2), std::round(size().y / 2 - active_factor) }, Color };
    arrow[1] = { { std::round(size().x / 2), std::round(size().y / 2 + active_factor) }, Color };
    arrow[2] = { { std::round(size().x / 2 + ArrowSize / 2), std::round(size().y / 2 - active_factor) }, Color };
    window.draw(arrow);
}

}
