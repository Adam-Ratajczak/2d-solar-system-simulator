#pragma once

#include "Button.hpp"

// FIXME: This button doesn't actually hide/show anything.
class HideShowButton : public Button {
public:
    explicit HideShowButton(Container& c);

private:
    virtual LengthVector initial_size() const override { return { Length::Auto, 20.0_px }; };
    virtual void draw(sf::RenderWindow&) const override;
};
