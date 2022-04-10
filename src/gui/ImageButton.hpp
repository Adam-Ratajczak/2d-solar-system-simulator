#pragma once

#include "Button.hpp"

class ImageButton : public Button
{
public:
    ImageButton(Container&, sf::Image);

    virtual void draw(sf::RenderWindow& window) const override;
    
private:
    sf::Texture m_texture;
};
