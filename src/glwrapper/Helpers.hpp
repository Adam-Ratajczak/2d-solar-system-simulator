#pragma once

#include <Essa/Engine/3D/Shaders/Basic.hpp>
#include <Essa/LLGL/Core/Transform.hpp>
#include <Essa/LLGL/OpenGL/Renderer.hpp>

namespace GL {

template<class Vertex>
void draw_with_temporary_vao(llgl::Renderer& renderer, llgl::ShaderImplPartial auto& shader, auto uniforms,
    llgl::PrimitiveType pt, std::span<Vertex const> vertices) {
    static llgl::VertexArray<Vertex> vao;
    vao.upload_vertices(vertices);
    renderer.draw_vertices(vao, llgl::DrawState { shader, uniforms, pt });
};

}
