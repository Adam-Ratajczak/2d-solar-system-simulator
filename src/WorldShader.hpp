#pragma once

#include <SFML/Graphics.hpp>

namespace WorldShader {

// Attributes:
// - layout (location = 0) in vec4 position;
// - layout (location = 1) in vec4 color;
// - layout (location = 2) in vec4 texCoords;

// Uniforms:
// - uniform mat4 projectionMatrix;
// - uniform mat4 worldViewMatrix;
// + defaults from <EssaGUI/gfx/SFMLWindow.hpp>
sf::Shader& world_shader();

}
