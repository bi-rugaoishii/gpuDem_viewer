// Camera.cpp
#include "Camera.h"
#include <cmath>
#include <algorithm>
#include <glm/gtc/constants.hpp>

Camera::Camera(glm::vec3 position)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5f), MouseSensitivity(0.1f), Yaw(-90.0f), Pitch(0.0f) {
    Position = position;
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessMouseScroll(float depthOffset){
    Zoom *= std::pow(0.85f,depthOffset);
    Zoom = std::clamp(Zoom,0.05f,120.0f);;
    /*
    if (Zoom <1.0f){
        Zoom = 1.0f;
    }
    if (Zoom > 45.0f){
        Zoom = 45.0f;
    }
    */
    //updateCameraVectors();
}
void Camera::ProcessKeyboard(Camera_Movement input, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (input == UP)
        Position += Up * velocity;
    if (input == DOWN)
        Position -= Up * velocity;
    if (input == LEFT)
        Position -= Right * velocity;
    if (input == RIGHT)
        Position += Right * velocity;

}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;
    Yaw += xoffset;
    Pitch += yoffset;

    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
