#pragma once

#include <GLES3/gl3.h>

namespace meshmedic::rendering {

class CubeMesh {
public:
    CubeMesh() = default;
    ~CubeMesh();

    CubeMesh(const CubeMesh&) = delete;
    CubeMesh& operator=(const CubeMesh&) = delete;

    bool initialize();
    void destroy();
    void draw() const;

private:
    GLuint vertex_array_ = 0;
    GLuint vertex_buffer_ = 0;
    GLuint index_buffer_ = 0;
    GLsizei index_count_ = 0;
};

} // namespace meshmedic::rendering
