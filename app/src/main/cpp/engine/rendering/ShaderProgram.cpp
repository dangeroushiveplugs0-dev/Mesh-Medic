#include "ShaderProgram.h"

namespace meshmedic::rendering {

ShaderProgram::~ShaderProgram() {
    destroy();
}

GLuint ShaderProgram::compile(GLenum type, std::string_view source) {
    const GLuint shader = glCreateShader(type);
    if (shader == 0) {
        return 0;
    }

    const char* source_data = source.data();
    const GLint source_length = static_cast<GLint>(source.size());
    glShaderSource(shader, 1, &source_data, &source_length);
    glCompileShader(shader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool ShaderProgram::initialize(
    std::string_view vertex_source,
    std::string_view fragment_source) {
    destroy();

    const GLuint vertex_shader = compile(GL_VERTEX_SHADER, vertex_source);
    const GLuint fragment_shader = compile(GL_FRAGMENT_SHADER, fragment_source);

    if (vertex_shader == 0 || fragment_shader == 0) {
        if (vertex_shader != 0) {
            glDeleteShader(vertex_shader);
        }
        if (fragment_shader != 0) {
            glDeleteShader(fragment_shader);
        }
        return false;
    }

    program_ = glCreateProgram();
    if (program_ == 0) {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return false;
    }

    glAttachShader(program_, vertex_shader);
    glAttachShader(program_, fragment_shader);
    glLinkProgram(program_);

    GLint linked = GL_FALSE;
    glGetProgramiv(program_, GL_LINK_STATUS, &linked);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    if (linked == GL_FALSE) {
        destroy();
        return false;
    }

    return true;
}

void ShaderProgram::destroy() {
    if (program_ != 0) {
        glDeleteProgram(program_);
        program_ = 0;
    }
}

void ShaderProgram::use() const {
    glUseProgram(program_);
}

GLint ShaderProgram::uniform_location(const char* name) const {
    return program_ == 0 ? -1 : glGetUniformLocation(program_, name);
}

} // namespace meshmedic::rendering
