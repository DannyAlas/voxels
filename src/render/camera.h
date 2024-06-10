#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.05f;
const float FOV         =  45.0f;

class Camera
{
public:
    // constructor with vectors
    Camera(
        glm::vec3 position = glm::vec3(0.0, 0.0, -5.0),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
        float yaw = YAW, 
        float pitch = PITCH) : 
            position(position), 
            worldUp(up), 
            yaw(yaw), 
            pitch(pitch), 
            front(glm::vec3(0.0f, 0.0f, -1.0f)), 
            movementSpeed(SPEED), 
            mouseSensitivity(SENSITIVITY)
    {
        updateCameraVectors();
    }

    // camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float movementSpeed;
    float mouseSensitivity;

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix(float width, float height);
    glm::mat4 getViewProjectionMatrix(float width, float height);
    void processKeyboard(Camera_Movement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    
private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(this->front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up    = glm::normalize(glm::cross(right, this->front));
    }
};

