// === main.cpp ===
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Sphere.h"
#include "CameraController.h"
#include "GuiManager.h"
#include "PositionLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

CameraController* gCamController = nullptr;
int gWindowWidth = 800;
int gWindowHeight = 600;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(gWindowWidth, gWindowHeight, "OOP Spheres with GUI", NULL, NULL);
    if (window == NULL) return -1;
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
    glEnable(GL_DEPTH_TEST);

    CameraController camController(glm::vec3(0.0f, 0.0f, 5.0f));
    camController.setAspectRatio(static_cast<float>(gWindowWidth)/static_cast<float>(gWindowHeight));
    gCamController = &camController;

    GuiManager gui;
    gui.init(window);

    Shader shader("vertex_shader.glsl", "fragment_shader.glsl");
    Sphere sphere(1.0f, 100, 100);
    float sphereScale = 0.03f;

    //read coordinates from a file
    PositionLoader loader;
    std::vector<FrameData> allFrames = loader.loadAllFrames("results");
    float timeAccumulator = 0.0f;
    float frameDuration = 0.05f; // 各フレームの持続時間（秒）

    std::vector<glm::vec3> spherePositions = allFrames.empty() ? std::vector<glm::vec3>() : allFrames[0].pos;

    sphereScale = allFrames[0].r[0];

    while (!glfwWindowShouldClose(window)) {
        gui.currentFrameTime = static_cast<float>(glfwGetTime());
        camController.updateTime(gui.currentFrameTime);
        camController.processKeyboard(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gui.beginFrame();
        int maxFrame = static_cast<int>(allFrames.size())-1;
        gui.drawSphereControl(sphereScale, camController.camera.Position,maxFrame);


        shader.use();

        camController.setAspectRatio(static_cast<float>(gWindowWidth)/static_cast<float>(gWindowHeight)); //for window size change
        glm::mat4 view = camController.camera.GetViewMatrix();
        glm::mat4 proj = camController.getProjectionMatrix(); //set perspective
        shader.setMat4("view", view);
        shader.setMat4("projection", proj);
        shader.setVec3("lightPos", glm::vec3(3.0f, 3.0f, 3.0f));
        shader.setVec3("viewPos", camController.camera.Position);

        //refresh animated frame
        gui.deltaTime = gui.currentFrameTime - gui.lastFrameTime;
        gui.lastFrameTime = gui.currentFrameTime;
        timeAccumulator += gui.deltaTime;

        //show in currentFrame
        spherePositions = allFrames[gui.currentFrame].pos;
        if (timeAccumulator >= frameDuration && !allFrames.empty() && gui.isPlayAnimation) {
            timeAccumulator = 0.0f;
            gui.currentFrame = (gui.currentFrame + 1) % allFrames.size();
            spherePositions = allFrames[gui.currentFrame].pos;
        }

        for (size_t i=0; i < spherePositions.size(); i++){
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(glm::mat4(1.0f),spherePositions[i]);
            //model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(sphereScale));
            shader.setMat4("model", model);
            sphere.draw();
        }

        gui.render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    gui.shutdown();
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    gWindowWidth = width;
    gWindowHeight = height;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow*, double xpos, double ypos) {
    if (!ImGui::GetIO().WantCaptureMouse && gCamController != nullptr)
        gCamController->mouseCallback(static_cast<float>(xpos), static_cast<float>(ypos));
}

void mouse_button_callback(GLFWwindow*, int button, int action, int) {
    if (!ImGui::GetIO().WantCaptureMouse && gCamController != nullptr)
        gCamController->mouseButtonCallback(button, action);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    if (!ImGui::GetIO().WantCaptureMouse && gCamController != nullptr)
        gCamController->scroll_callback(static_cast<float>(yoffset));

}
