#include "Triangle.hpp"
#include <iostream>

void Triangle::draw(sf::RenderWindow &window) const{
    window.popGLStates();
    
	glBegin(GL_TRIANGLES);
    {
        v1.color.glDraw();
        v1.position.glDraw();

        v2.color.glDraw();
        v2.position.glDraw();

        v3.color.glDraw();
        v3.position.glDraw();
    }
    glEnd();
    
    window.pushGLStates();
}
