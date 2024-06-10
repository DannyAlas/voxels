#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>


class Shader {
public:
    Shader();
    ~Shader();

    void bind() const;
    void unbind() const;

    void compile(unsigned int type, const std::string &filepath);
    void link();
    unsigned int getProgram() const;

    void setUniform(const std::string &name, const glm::vec2 &value) const;
    void setUniform(const std::string &name, const glm::vec3 &value) const;
    void setUniform(const std::string &name, const glm::mat4 &value) const;
    void setUniform(const std::string &name, float value) const;
    void setUniform(const std::string &name, int value) const;

    void setCameraPosition(const glm::vec3 &position) const;
    void setCameraDirection(const glm::vec3 &direction) const;
    void setResolution(int width, int height) const;
    void setCameraMatrix(const glm::mat4 &MVP) const;
    void setTime(float time) const;

private:
    unsigned int m_program;
    std::vector<unsigned int> shaders;

    std::string readFile(const std::string &filepath) const;
};

