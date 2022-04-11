#include "ImageButton.hpp"

namespace GUI {

ImageButton::ImageButton(Container& c, sf::Image img)
    : Button(c) {
    m_texture.loadFromImage(img);
    m_texture.setSmooth(true);
}

void ImageButton::draw(sf::RenderWindow& window) const {
    sf::CircleShape cs_bg(0.5);
    cs_bg.setScale(size());
    cs_bg.setFillColor(bg_color_for_state());
    window.draw(cs_bg);

    sf::Sprite sprite;
    sprite.setTexture(m_texture);

    auto tex_size = sprite.getTexture()->getSize();
    sprite.setScale(size().x / tex_size.x, size().y / tex_size.y);

    window.draw(sprite);
}

}
