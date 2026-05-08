
// CameraController.cpp
#define GLM_ENABLE_EXPERIMENTAL
#include "CameraController.h"
#include <cstdio>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/quaternion.hpp>

CameraController::CameraController(glm::vec3 startPos)
    : camera(startPos), lastX(400.0f), lastY(300.0f), firstMouse(true), rotatingCamera(false), deltaTime(0.0f), lastFrame(0.0f), target(0.0f,0.0f,0.0f) {}

void CameraController::updateTime(float currentTime) {
    deltaTime = currentTime - lastFrame;
    lastFrame = currentTime;
}

void CameraController::processKeyboard(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

glm::vec3 CameraController::safeNormalize(const glm::vec3& v){
    float len = glm::length(v);
    if(len < 1e-6f) return glm::vec3(0.0f, 1.0f, 0.0f); //stop zero div

    return v/len;
}

void CameraController::mouseCallback(float xpos, float ypos)
{
    if (!rotatingCamera) return;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float dx = xpos - lastX;
    float dy = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    float speed = 0.005f;

    float yawAngle   = -dx * speed;
    float pitchAngle = -dy * speed;

    float distance = glm::length(target - camera.Position);
    glm::vec3 pivot = camera.Position + camera.Front * distance;
    target = pivot;

    glm::vec3 dir = camera.Position - pivot;

    glm::quat qYaw   = glm::angleAxis(yawAngle, camera.Up);
    glm::quat qPitch = glm::angleAxis(pitchAngle, camera.Right);

    glm::quat q = qPitch*qYaw;

    dir = q * dir;

    camera.Position = pivot + dir;

    camera.Front = safeNormalize(pivot - camera.Position);

    camera.Right = safeNormalize(glm::cross(camera.Front, camera.Up));

    camera.Up    = safeNormalize(glm::cross(camera.Right, camera.Front));
}

void CameraController::mouseButtonCallback(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            rotatingCamera = true;
            firstMouse = true;
        } else if (action == GLFW_RELEASE) {
            rotatingCamera = false;
        }
    }
}


void CameraController::setAspectRatio(float aspect){
    this->aspectRatio = aspect;
}

void CameraController::scroll_callback(float yoffset){
    camera.ProcessMouseScroll(yoffset);
}

glm::mat4 CameraController::getProjectionMatrix() const{
    return glm::perspective(glm::radians(camera.Zoom), aspectRatio, 0.1f, 100.0f);
}
