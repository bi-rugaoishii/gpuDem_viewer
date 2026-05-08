// CameraController.h
#pragma once
#include "Camera.h"
#include <GLFW/glfw3.h>

class CameraController {
public:
    glm::vec3 target;
    Camera camera;
    CameraController(glm::vec3 startPos);
    void updateTime(float currentTime);
    void setAspectRatio(float aspect);
    void processKeyboard(GLFWwindow* window);
    void mouseCallback(float xpos, float ypos);
    void scroll_callback(float yoffset);
    void mouseButtonCallback(int button, int action);
    glm::mat4 getProjectionMatrix() const;
    glm::vec3 safeNormalize(const glm::vec3& v);

    float lastX;
    float lastY;
    bool firstMouse;
    bool rotatingCamera;
    float deltaTime;
    float lastFrame;
    float aspectRatio;
};

