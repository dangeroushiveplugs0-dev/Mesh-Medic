#include "CubeMesh.h"

namespace meshmedic::rendering {

namespace {

struct Vertex {
    float position[3];
    float color[3];
};

constexpr Vertex vertices[] = {
    {{-1.0f, -1.0f,  1.0f}, {1.0f, 0.2f, 0.2f}},
    {{ 1.0f, -1.0f,  1.0f}, {0.2f, 1.0f, 0.2f}},
    {{ 1.0f,  1.0f,  1.0f}, {0.2f, 0.6f, 1.0f}},
    {{-1.0f,  1.0f,  1.0f}, {1.0f, 0.8f, 0.2f}},
    {{-1.0f, -1.0f, -1.0f}, {0.8f, 0.2f, 1.0f}},
    {{ 1.0f, -1.0f, -1.0f}, {0.2f, 0.9f, 1.0f}},
    {{ 1.0f,  1.0f, -1.0f}, {1.0f, 0.5f, 0.2f}},
    {{-1.0f,  1.0f, -1.0f}, {0.7f, 0.7f, 0.8f}}
};

constexpr GLuint indices[] = {
    0, 1, 2, 2, 3, 0,
    1, 5, 6, 6, 2, 1,
    5, 4, 7, 7, 6, 5,
    4, 0, 3, 3, 7, 4,
    3, 2, 6, 6, 7, 3,
    4, 5, 1, 1, 0, 4
};

} // namespace

CubeMesh::~CubeMesh() {
    destroy();
}

bool CubeMesh::initialize() {
    destroy();

    glGenVertexArrays(1, &vertex_array_);
    glGenBuffers(1, &vertex_buffer_);
    glGenBuffers(1, &index_buffer_);

    if (vertex_array_ == 0 || vertex_buffer_ == 0 || index_buffer_ == 0) {
        destroy();
        return false;
    }

    glBindVertexArray(vertex_array_);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<const void*>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<const void*>(sizeof(float) * 3));

    glBindVertexArray(0);

    index_count_ = static_cast<GLsizei>(sizeof(indices) / sizeof(indices[0]));
    return glGetError() == GL_NO_ERROR;
}

void CubeMesh::destroy() {
    if (index_buffer_ != 0) {
        glDeleteBuffers(1, &index_buffer_);
        index_buffer_ = 0;
    }
    if (vertex_buffer_ != 0) {
        glDeleteBuffers(1, &vertex_buffer_);
        vertex_buffer_ = 0;
    }
    if (vertex_array_ != 0) {
        glDeleteVertexArrays(1, &vertex_array_);
        vertex_array_ = 0;
    }
    index_count_ = 0;
}

void CubeMesh::draw() const {
    if (vertex_array_ == 0 || index_count_ == 0) {
        return;
    }

    glBindVertexArray(vertex_array_);
    glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

} // namespace meshmedic::rendering
