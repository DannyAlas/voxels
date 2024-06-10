#include "renderer.h"
#include "voxel.h"
#include <glm/fwd.hpp>
#include <iostream>
#include <ostream>
#include <random>
#include "glfw/glfw.h"

// Example quad vertices for rendering
float quadVertices[] = {
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

Renderer::Renderer() {
    m_camera = std::make_unique<Camera>();
    m_shader = std::make_unique<Shader>();
    SVO svo;
    initializeOctree();

    // Setup VAO and VBO for rendering the quad
    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);

    // Compile and link shader
    m_shader->compile(GL_VERTEX_SHADER, "../res/shaders/vertex.glsl");
    m_shader->compile(GL_FRAGMENT_SHADER, "../res/shaders/voxel.frag");
    m_shader->link();
    m_shader->bind();

    // Set initial uniforms
    m_shader->setCameraPosition(m_camera->position);
    m_shader->setCameraMatrix(m_camera->getViewMatrix());
    std::cout << "Renderer initialized" << std::endl;
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &ssbo);
}


void Renderer::initializeOctree() {
    int cubeSize = 10;
    int halfCubeSize = cubeSize / 2;

    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    // Calculate the center of the current octree
    Vec3i32 center = (svo.maxIncl() + svo.minIncl()) / 2;

    // Calculate the bounds of the cube
    Vec3i32 minBound = center - Vec3i32(halfCubeSize);
    Vec3i32 maxBound = center + Vec3i32(halfCubeSize - 1);

    // Insert voxels in the calculated bounds with random pastel colors
    for (int x = minBound.x; x <= maxBound.x; ++x) {
        for (int y = minBound.y; y <= maxBound.y; ++y) {
            for (int z = minBound.z; z <= maxBound.z; ++z) {
                // Generate a random pastel color
                uint8_t r = dis(gen) / 2 + 128;  // Pastel color (lighter shade)
                uint8_t g = dis(gen) / 2 + 128;
                uint8_t b = dis(gen) / 2 + 128;
                rgb32_t color = glm::uvec4(r, g, b, 255);

                svo.insert(Vec3i32(x, y, z), color);
            }
        }
    }
    // Flatten the octree and update the SSBO
    updateSSBO();
}

void Renderer::updateSSBO() {
    std::vector<uint32_t> buffer;
    svo.flatten(buffer);

    std::cout << "Flattened octree with buffer size: " << buffer.size() << std::endl;

    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, buffer.size() * sizeof(uint32_t), buffer.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
}

void Renderer::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    int width, height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
    glViewport(0, 0, width, height);
    glm::mat4 matrix_og = m_camera->getViewProjectionMatrix(width, height);
    auto inverse = glm::inverse(matrix_og);
    m_shader->setUniform("matrix_original", matrix_og);
    m_shader->setUniform("inverse_matrix", inverse);
    m_shader->setTime(glfwGetTime());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
