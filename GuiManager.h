// GuiManager.h
#pragma once
#include "imgui.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class GuiManager {
public:
    void init(GLFWwindow* window);
    void beginFrame();
    void drawSphereControl(float& scale, const glm::vec3& cameraPos,const int maxFrame);
    void render();
    void shutdown();

    //For animation
    bool isPlayAnimation=false;
    float lastFrameTime=0;
    float deltaTime=0;
    int currentFrame =0;
    float currentFrameTime =0.;

    // --- frame button control ---
    float holdTimerLeft = 0.0f;
    float holdTimerRight = 0.0f;
    float holdInterval = 1.0f / 6.0f; // 6fps
    float holdDelay = 0.25f;

};

