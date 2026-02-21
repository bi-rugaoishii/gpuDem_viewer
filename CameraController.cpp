
// CameraController.cpp
#define GLM_ENABLE_EXPERIMENTAL
#include "CameraController.h"
#include <glm/gtx/rotate_vector.hpp>

CameraController::CameraController(glm::vec3 startPos)
    : camera(startPos), lastX(400.0f), lastY(300.0f), firstMouse(true), rotatingCamera(false), deltaTime(0.0f), lastFrame(0.0f) {}

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

void CameraController::mouseCallback(float xpos, float ypos) {
    if (!rotatingCamera) return;
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float angleSpeed = 0.005f;
    float horizontalAngle = xoffset*angleSpeed;
    float verticalAngle = yoffset*angleSpeed;

    float focusDistance = 5.0f;
    target = camera.Position + camera.Front * focusDistance;

    glm::vec3 direction = camera.Position - target;
    direction = glm::rotate(direction, -horizontalAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 right = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
    direction = glm::rotate(direction, -verticalAngle, right);

    camera.Position = target + direction;
    camera.Front = glm::normalize(target - camera.Position);
    camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
    camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));

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
