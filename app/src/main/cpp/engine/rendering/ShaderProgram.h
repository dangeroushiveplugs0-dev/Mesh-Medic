#pragma once

#include <GLES3/gl3.h>

#include <string_view>

namespace meshmedic::rendering {

class ShaderProgram {
public:
    ShaderProgram() = default;
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    bool initialize(std::string_view vertex_source, std::string_view fragment_source);
    void destroy();
    void use() const;
    GLint uniform_location(const char* name) const;

    GLuint id() const { return program_; }

private:
    static GLuint compile(GLenum type, std::string_view source);

    GLuint program_ = 0;
};

} // namespace meshmedic::rendering
