// === main.cpp ===
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Sphere.h"
#include "CameraController.h"
#include "GuiManager.h"
#include "PositionLoader.h"
#include "SphereSmooth.h"
#include "StlMeshLoader.h"
#include "Color.h"
#include "ImGuiFileDialog.h"
#include "Mesh.h"
//#include "FileDialog.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>

GLuint instanceVBO;
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
 //   Shader fastShader("fast_vertex.glsl", "fast_fragment_shader.glsl");

 // ===== Shader =====
    Shader meshShader("smooth_vertex_shader.glsl", "smooth_fragment_shader.glsl");

    // ===== Sphere =====
    SphereSmooth sphere(1.0f, 10, 10);

    // ===== Instance VBO =====
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*1000000, NULL, GL_DYNAMIC_DRAW);

    // Sphere VAO に instance を紐付け
    sphere.bind();
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);

    //* ==== result folder names === */
    std::vector<std::string> resultNames;

    // ===== STL関連 =====
    std::vector<Mesh> stlMeshes;
    std::vector<std::string> stlNames;


    //read coordinates from a file
    PositionLoader loader;

    
    std::vector<std::vector<FrameData>> allFrames;
    resultNames.push_back("results"); // default name
    allFrames.push_back(loader.loadAllFrames("results"));

    float timeAccumulator = 0.0f;
    float frameDuration = 0.05f; // 各フレームの持続時間（秒）

    std::vector<std::vector<glm::vec3>> spherePositions;
    std::vector<std::vector<float>> radius;



    /* ======= main render loop =========== */
    while (!glfwWindowShouldClose(window)) {
        gui.currentFrameTime = static_cast<float>(glfwGetTime());
        camController.updateTime(gui.currentFrameTime);
        camController.processKeyboard(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gui.beginFrame();

        /* == reset spherePositions and radius == */
        spherePositions.clear() ;
        radius.clear() ;

        int maxFrame = std::numeric_limits<size_t>::max();
        for(size_t i=0; i<allFrames.size(); i++){
            if(allFrames[i].size()<maxFrame){
                maxFrame=allFrames[i].size();
            }
        }

        maxFrame = maxFrame-1;

        /* ============= GUI buttons ============== */
        gui.drawSphereControl(camController.camera.Position,maxFrame);
        gui.drawStlControl(stlNames);
        gui.drawResultControl(resultNames);


        /* ========= result loading ============= */
        // =======================================
        // Result Folder Dialog
        // =======================================

        if(gui.openResultDialog){

            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog(
                    "ChooseResult",
                    "Choose Result Folder",
                    nullptr,
                    config);
            gui.openResultDialog = false;
        }

        if(ImGuiFileDialog::Instance()->Display("ChooseResult")){

            if(ImGuiFileDialog::Instance()->IsOk())
            {
                std::string folderPath =
                    ImGuiFileDialog::Instance()->GetCurrentPath();
                allFrames.push_back(loader.loadAllFrames(folderPath));
                resultNames.push_back(folderPath);

                /* == refresh max frame to the smallest of loaded results == */
                printf("checking minimum of maximum frames\n");
                maxFrame = std::numeric_limits<size_t>::max();
                for(size_t i=0; i<allFrames.size(); i++){
                    if(allFrames[i].size()<maxFrame){
                        maxFrame=allFrames[i].size();
                    }
                }
                maxFrame = maxFrame-1;

                printf("checking done\n");

                gui.drawSphereControl(camController.camera.Position,maxFrame);
                if(maxFrame<gui.currentFrame){
                    gui.currentFrame=maxFrame;
                }
            }

            ImGuiFileDialog::Instance()->Close();
        }

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

                    auto tris = loader.getTriangles();


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
        }

        if(gui.requestStlDelete >= 0){
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

            allFrames.clear();
            for (size_t i=0; i<resultNames.size(); i++){
                allFrames.push_back(loader.loadAllFrames(resultNames[i]));
            }

            if(!allFrames.empty())
            {
                /* == refresh max frame to the smallest of loaded results == */
                maxFrame = 1e16;
                for(size_t i=0; i<allFrames.size(); i++){
                    if(allFrames[i].size()<maxFrame){
                        maxFrame=allFrames[i].size();
                    }
                }

                if(maxFrame<gui.currentFrame){
                    gui.currentFrame=maxFrame;

                }
                maxFrame = maxFrame - 1;

                spherePositions.clear() ;
                radius.clear() ;
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

        spherePositions.reserve(allFrames.size());
        radius.reserve(allFrames.size());
        for (size_t i=0; i<allFrames.size(); i++){

            std::vector<glm::vec3> const &tmp_pos =allFrames[i][gui.currentFrame].pos; 
            spherePositions.push_back(tmp_pos);

            std::vector<float> const &tmp_radius = allFrames[i][gui.currentFrame].r;
            radius.push_back(tmp_radius);
        }

        if (timeAccumulator >= frameDuration && !allFrames.empty() && gui.isPlayAnimation) {
            timeAccumulator = 0.0f;
            gui.currentFrame = (gui.currentFrame + 1) % maxFrame;
        }

        // =========================
        //  Sphere Instancing 描画
        // =========================
        meshShader.use();

        meshShader.setMat4("view", view);
        meshShader.setMat4("projection", proj);
        meshShader.setVec3("lightPos", camController.camera.Position);
        meshShader.setVec3("viewPos", camController.camera.Position);

        sphere.bind();

        for(size_t i=0;i<spherePositions.size();i++)
        {
            std::vector<float> instanceData;
            instanceData.reserve(spherePositions[i].size()*4);

            for(size_t j=0;j<spherePositions[i].size();j++){
                instanceData.push_back(spherePositions[i][j].x);
                instanceData.push_back(spherePositions[i][j].y);
                instanceData.push_back(spherePositions[i][j].z);
                instanceData.push_back(radius[i][j]);
            }

            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0,
                    instanceData.size()*sizeof(float),
                    &instanceData[0]);

            // 色
            Color color;
            color.make_hue(i);
            meshShader.setVec3("objectColor",
                    glm::vec3(color.r, color.g, color.b));

            glDrawElementsInstanced(
                    GL_TRIANGLES,
                    sphere.getIndexCount(),
                    GL_UNSIGNED_INT,
                    0,
                    spherePositions[i].size()
                    );
        }

        /* === display stl === */
        shader.use();

        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // back face
        glDepthMask(GL_FALSE);
        glCullFace(GL_FRONT);

        float opacity = 0.4f;
        for(size_t i=0;i<stlMeshes.size();i++)
        {
            glm::mat4 model(1.0f);

            shader.setMat4("model",model);
            shader.setFloat("opacity",opacity);
            shader.setVec3("objectColor",glm::vec3(0.7f,0.7f,0.7f));

            stlMeshes[i].draw();
        }

        // front face
        glCullFace(GL_BACK);
        for(size_t i=0;i<stlMeshes.size();i++)
        {
            glm::mat4 model(1.0f);

            shader.setMat4("model",model);
            shader.setFloat("opacity",opacity);
            shader.setVec3("objectColor",glm::vec3(0.7f,0.7f,0.7f));

            stlMeshes[i].draw();
        }
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);

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
