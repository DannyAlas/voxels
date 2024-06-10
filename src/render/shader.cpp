
#include "shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#ifdef SHDEBUG
#define DEBUG_LOG(msg) std::cerr << msg << std::endl
#else
#define DEBUG_LOG(msg)
#endif

std::string Shader::readFile(const std::string &filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return "";
    }
    std::ostringstream buf;
    buf << file.rdbuf();
    DEBUG_LOG("Read file: " << filepath << " with " << buf.str().size() << " bytes");
    // log file contents
    // DEBUG_LOG("File contents: " << buf.str());
    return buf.str();
}

Shader::Shader() : m_program(glCreateProgram()) {
    if (m_program == 0) {
        std::cerr << "Failed to create shader program." << std::endl;
    }
    DEBUG_LOG("Created shader program with ID: " << m_program);
}

Shader::~Shader() {
    if (m_program != 0) {
        glDeleteProgram(m_program);
    }
}

void Shader::bind() const {
    glUseProgram(m_program);
    if (glGetError() != GL_NO_ERROR) {
        std::cerr << "Failed to bind shader program." << std::endl;
    }
    // DEBUG_LOG("Bound shader program with ID: " << m_program);
}

void Shader::unbind() const {
    glUseProgram(0);
    DEBUG_LOG("Unbound shader program with ID: " << m_program);
}

void Shader::compile(unsigned int type, const std::string &filepath) {
    std::string source = readFile(filepath);
    if (source.empty()) {
        std::cerr << "Failed to read shader source from file: " << filepath << std::endl;
        throw std::runtime_error("Failed to read shader source from file: " + filepath);
        return;
    }
    DEBUG_LOG("Compiling shader from file: " << filepath);

    unsigned int shader = glCreateShader(type);
    const char *sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    int isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    DEBUG_LOG("Compiled shader with ID: " << shader << " and type: " << type);
    if (isCompiled == GL_TRUE) {
        shaders.push_back(shader);
        DEBUG_LOG("Shader compilation successful!");
    } else {
        DEBUG_LOG("Shader compilation failure!");
        int maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<char> infoLog((size_t)maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog.data());

        std::cerr << "Shader compilation failure!" << std::endl
                  << infoLog.data() << std::endl;
        glDeleteShader(shader);
        throw std::runtime_error("Shader compilation failure!");
    }
}

void Shader::link() {
    for (auto shader : shaders) {
        glAttachShader(m_program, shader);
        DEBUG_LOG("Attached shader with ID: " << shader);
    }

    glLinkProgram(m_program);

    int isLinked = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        int maxLength = 0;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<char> infoLog((size_t)maxLength);
        glGetProgramInfoLog(m_program, maxLength, &maxLength, infoLog.data());

        std::cerr << "Shader link failure!" << std::endl
                  << infoLog.data() << std::endl;
    } else {
        DEBUG_LOG("Shader link successful!");
        for (auto shader : shaders) {
            glDetachShader(m_program, shader);
            glDeleteShader(shader);
            DEBUG_LOG("Detached and deleted shader with ID: " << shader);
        }
    }

    shaders.clear();
}

unsigned int Shader::getProgram() const {
    return m_program;
}

void Shader::setUniform(const std::string &name, const glm::vec2 &value) const {
    int location = glGetUniformLocation(m_program, name.c_str());
    if (location == -1) {
        std::cerr << "Failed to find uniform location for '" << name << "'" << std::endl;
        return;
    }
    // DEBUG_LOG("Setting uniform " << name << " to " << value.x << ", " << value.y);
    glUniform2fv(location, 1, glm::value_ptr(value));
}

void Shader::setUniform(const std::string &name, const glm::vec3 &value) const {
    int location = glGetUniformLocation(m_program, name.c_str());
    if (location == -1) {
        std::cerr << "Failed to find uniform location for '" << name << "'" << std::endl;
        return;
    }
    // DEBUG_LOG("Setting uniform " << name << " to " << value.x << ", " << value.y << ", " << value.z);
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::setUniform(const std::string &name, const glm::mat4 &value) const {
    int location = glGetUniformLocation(m_program, name.c_str());
    if (location == -1) {
        std::cerr << "Failed to find uniform location for '" << name << "'" << std::endl;
        return;
    }
    // DEBUG_LOG("Setting uniform " << name << " to ");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniform(const std::string &name, float value) const {
    int location = glGetUniformLocation(m_program, name.c_str());
    if (location == -1) {
        std::cerr << "Failed to find uniform location for '" << name << "'" << std::endl;
        return;
    }
    // DEBUG_LOG("Setting uniform " << name << " to " << value);
    glUniform1f(location, value);
}

void Shader::setUniform(const std::string &name, int value) const {
    int location = glGetUniformLocation(m_program, name.c_str());
    if (location == -1) {
        std::cerr << "Failed to find uniform location for '" << name << "'" << std::endl;
        return;
    }
    // DEBUG_LOG("Setting uniform " << name << " to " << value);
    glUniform1i(location, value);
}

void Shader::setCameraMatrix(const glm::mat4& MVP) const {
    setUniform("cameraMatrix", MVP);
}

void Shader::setCameraPosition(const glm::vec3 &position) const {
    setUniform("cameraPosition", position);
}

void Shader::setResolution(int width, int height) const {
    setUniform("resolution", glm::vec2(width, height));
}

void Shader::setTime(float time) const {
    setUniform("time", time);
}
