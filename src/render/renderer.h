#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "camera.h"
#include "voxel.h"

class Renderer {
public:
    Renderer();
    ~Renderer();
    void render();    
    Camera* camera() { return m_camera.get(); }
    Shader* shader() { return m_shader.get(); }

private:
    std::unique_ptr<Shader> m_shader; 
    std::unique_ptr<Camera> m_camera;

    GLuint ssbo;
    GLuint VAO;
    SVO svo;

    void initializeOctree();
    void updateSSBO();
};
