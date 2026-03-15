// === main.cpp ===
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Sphere.h"
#include "CameraController.h"
#include "GuiManager.h"
#include "PositionLoader.h"
#include "StlMeshLoader.h"
#include "ImGuiFileDialog.h"
#include "Mesh.h"
//#include "FileDialog.h"
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
    glEnable(GL_PROGRAM_POINT_SIZE);  //added
    glEnable(GL_BLEND);  //added
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    CameraController camController(glm::vec3(0.0f, 0.0f, 5.0f));
    camController.setAspectRatio(static_cast<float>(gWindowWidth)/static_cast<float>(gWindowHeight));
    gCamController = &camController;

    GuiManager gui;
    gui.init(window);

    Shader shader("vertex_shader.glsl", "fragment_shader.glsl");
    Shader fastShader("fast_vertex.glsl", "fast_fragment_shader.glsl");

    Sphere sphere(1.0f, 100, 100);
    float sphereScale = 0.03f;

    // ===== STL関連 =====
    std::vector<Mesh> stlMeshes;
    std::vector<std::string> stlNames;

    //////
    unsigned int pointVAO, pointVBO;
    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);

    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);

    // 最大粒子数ぶん確保（仮に100万想定）
    glBufferData(GL_ARRAY_BUFFER,
            sizeof(float)*4*1000000,
            nullptr,
            GL_DYNAMIC_DRAW);

    // location 0 = position
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,
            sizeof(float)*4,(void*)0);
    glEnableVertexAttribArray(0);

    // location 1 = radius
    glVertexAttribPointer(1,1,GL_FLOAT,GL_FALSE,
            sizeof(float)*4,(void*)(sizeof(float)*3));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    ///

    //read coordinates from a file
    PositionLoader loader;
    std::vector<FrameData> allFrames = loader.loadAllFrames("results");
    std::vector<FrameData> allFrames2 = loader.loadAllFrames("results2");
    bool hasSecondDataset = !allFrames2.empty();

    float timeAccumulator = 0.0f;
    float frameDuration = 0.05f; // 各フレームの持続時間（秒）

    std::vector<glm::vec3> spherePositions = allFrames.empty() ? std::vector<glm::vec3>() : allFrames[0].pos;
    std::vector<glm::vec3> spherePositions2 = allFrames2.empty() ? std::vector<glm::vec3>() : allFrames2[0].pos;

    sphereScale = allFrames[0].r[0];


    while (!glfwWindowShouldClose(window)) {
        gui.currentFrameTime = static_cast<float>(glfwGetTime());
        camController.updateTime(gui.currentFrameTime);
        camController.processKeyboard(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gui.beginFrame();
        int maxFrame = 0;
        if(hasSecondDataset){

            maxFrame = std::min(static_cast<int>(allFrames.size()),static_cast<int>(allFrames2.size()))-1;
        }else{
            maxFrame = static_cast<int>(allFrames.size())-1;
        }

        /* ============= GUI buttons ============== */
        gui.drawSphereControl(sphereScale, camController.camera.Position,maxFrame);
        gui.renderModeControl();
        gui.drawStlControl(stlNames);


        /* ========= stl loading ============= */
        // =======================================
        // STL File Dialog
        // =======================================

        if(gui.openStlDialog)
        {
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog(
                    "ChooseSTL",
                    "Choose STL File",
                    ".stl",
                    config);
            gui.openStlDialog = false;
        }

        if(ImGuiFileDialog::Instance()->Display("ChooseSTL"))
        {
            if(ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePath =
                    ImGuiFileDialog::Instance()->GetFilePathName();

                // ===== STLロード処理 =====
                StlMeshLoader loader;

                if(loader.loadASCII(filePath))
                {
                    std::vector<glm::vec3> vertices;
                    std::vector<glm::vec3> normals;

                    const auto& tris = loader.getTriangles();

                    for(size_t i=0;i<tris.size();i++)
                    {
                        vertices.push_back(tris[i].v0);
                        vertices.push_back(tris[i].v1);
                        vertices.push_back(tris[i].v2);

                        normals.push_back(tris[i].normal);
                        normals.push_back(tris[i].normal);
                        normals.push_back(tris[i].normal);
                    }

                    Mesh mesh;
                    mesh.build(vertices,normals);

                    stlMeshes.push_back(mesh);
                    stlNames.push_back(filePath);
                }
            }

            ImGuiFileDialog::Instance()->Close();
        }if(gui.requestStlDelete >= 0)
        {
            int idx = gui.requestStlDelete;
            gui.requestStlDelete = -1;

            if(idx < stlMeshes.size())
            {
                stlMeshes.erase(stlMeshes.begin()+idx);
                stlNames.erase(stlNames.begin()+idx);
            }
        }

        // ======================================
        // Reload request handling
        // ======================================
        if(gui.requestReload)
        {
            gui.requestReload = false;

            allFrames = loader.loadAllFrames("results");

            if(!allFrames.empty())
            {
                gui.currentFrame = 0;
                spherePositions = allFrames[0].pos;
                sphereScale = allFrames[0].r[0];
            }

            allFrames2 = loader.loadAllFrames("results2");

            if(!allFrames2.empty()){
                hasSecondDataset = true;
            }else{
                hasSecondDataset = false;
            }
        }

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
        if(hasSecondDataset){
            spherePositions2 = allFrames2[gui.currentFrame].pos;
        }

        if (timeAccumulator >= frameDuration && !allFrames.empty() && gui.isPlayAnimation) {
            timeAccumulator = 0.0f;
            gui.currentFrame = (gui.currentFrame + 1) % allFrames.size();
            spherePositions = allFrames[gui.currentFrame].pos;
        }
        if(gui.isFastPointMode)
        {
            fastShader.use();

            glm::mat4 view = camController.camera.GetViewMatrix();
            glm::mat4 proj = camController.getProjectionMatrix();

            fastShader.setMat4("view", view);
            fastShader.setMat4("projection", proj);
            fastShader.setFloat("viewportHeight",(float)gWindowHeight);
            fastShader.setFloat("sphereScale",sphereScale);

            glBindVertexArray(pointVAO);
            glBindBuffer(GL_ARRAY_BUFFER, pointVBO);

            /* dataset 1*/
            {
                std::vector<float> gpuData;
                gpuData.reserve(spherePositions.size()*4);

                const std::vector<float>& r =
                    allFrames[gui.currentFrame].r;

                for(size_t i=0;i<spherePositions.size();i++)
                {
                    gpuData.push_back(spherePositions[i].x);
                    gpuData.push_back(spherePositions[i].y);
                    gpuData.push_back(spherePositions[i].z);
                    gpuData.push_back(r[i]); 
                }

                glBufferSubData(GL_ARRAY_BUFFER,0,
                        gpuData.size()*sizeof(float),
                        gpuData.data());



                fastShader.setVec3("objectColor", glm::vec3(1,1,1));
                glDrawArrays(GL_POINTS,0,spherePositions.size());
            }

            if(hasSecondDataset){
                std::vector<float> gpuData;
                gpuData.reserve(spherePositions2.size()*4);

                const std::vector<float>& r =
                    allFrames2[gui.currentFrame].r;

                for(size_t i=0;i<spherePositions2.size();i++)
                {
                    gpuData.push_back(spherePositions2[i].x);
                    gpuData.push_back(spherePositions2[i].y);
                    gpuData.push_back(spherePositions2[i].z);
                    gpuData.push_back(r[i]); 
                }

                glBufferSubData(GL_ARRAY_BUFFER,0,
                        gpuData.size()*sizeof(float),
                        gpuData.data());



                fastShader.setVec3("objectColor", glm::vec3(0.3,0.6,1.0));
                glDrawArrays(GL_POINTS,0,spherePositions2.size());
            }

        }else{
            for (size_t i=0; i < spherePositions.size(); i++){
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(glm::mat4(1.0f),spherePositions[i]);
                //model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::scale(model, glm::vec3(sphereScale));
                shader.setMat4("model", model);
                sphere.draw();
            }
        }


        shader.use();
        float opacity = 0.4f;
        for(size_t i=0;i<stlMeshes.size();i++)
        {
            glm::mat4 model(1.0f);

            shader.setMat4("model",model);
            shader.setFloat("opacity",opacity);
            shader.setVec3("objectColor",glm::vec3(0.7f,0.7f,0.7f));

            stlMeshes[i].draw();
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
