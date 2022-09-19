#pragma once

#include <EssaEngine/3D/Shaders/Basic.hpp>
#include <LLGL/Core/Transform.hpp>
#include <LLGL/OpenGL/Renderer.hpp>

namespace GL {

template<class Vertex>
void draw_with_temporary_vao(llgl::Renderer& renderer, llgl::ShaderImpl auto& shader,
    llgl::PrimitiveType pt, std::span<Vertex const> vertices) {
    static llgl::VertexArray<Vertex> vao;
    vao.upload_vertices(vertices);
    renderer.draw_vertices(vao, llgl::DrawState { shader, pt });
};

}
