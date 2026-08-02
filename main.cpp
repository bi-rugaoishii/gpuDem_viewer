// === main.cpp ===
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "PlaneRenderer.h"
#include "Sphere.h"
#include "CameraController.h"
#include "GuiManager.h"
#include "PositionLoader.h"
#include "StlMeshLoader.h"
#include "Color.h"
#include "ImGuiFileDialog.h"
#include "picojson.h"
#include "Mesh.h"
//#include "FileDialog.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <utility>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

CameraController* gCamController = nullptr;
int gWindowWidth = 800;
int gWindowHeight = 600;

int getMaxFrame(const std::vector<std::vector<std::string>>& allFrameFiles);
void loadCurrentFrames(PositionLoader& loader, const std::vector<std::vector<std::string>>& allFrameFiles, int frameIndex, std::vector<FrameData>& currentFrames, std::vector<int>& loadedFrameIndices);

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

    glm::vec3 startPos(0.0f,0.0f,5.0f);

    CameraController camController(startPos);
    camController.setAspectRatio(static_cast<float>(gWindowWidth)/static_cast<float>(gWindowHeight));
    gCamController = &camController;

    GuiManager gui;
    gui.init(window);

    std::string pathShader=VIEWER_SHADER_PATH;
    Shader shader(pathShader+"/vertex_shader.glsl", pathShader+"/fragment_shader.glsl");
    Shader fastShader(pathShader+"/fast_vertex.glsl",pathShader+"/fast_fragment_shader.glsl");

    Sphere sphere(1.0f, 100, 100);

    //* ==== result folder names === */
    std::vector<std::string> resultNames;

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


    const int invalidFrameIndex = -1;
    std::vector<std::vector<std::string>> allFrameFiles;
    std::vector<FrameData> currentFrames;
    std::vector<int> loadedFrameIndices;

    resultNames.push_back("results");
    allFrameFiles.push_back(loader.listFrameFiles("results"));
    currentFrames.resize(allFrameFiles.size());
    loadedFrameIndices.resize(allFrameFiles.size(), invalidFrameIndex);

    float timeAccumulator = 0.0f;
    float frameDuration = 1.0f/20.0f; // 各フレームの持続時間（秒）


    //read demSettings.json
    std::ifstream demSettings("demSettings.json");
    if (!demSettings){
        std::printf("demSettings.json not found!!!!\n");
    }else{
        picojson::value jsonValue;

        std::string err = picojson::parse(jsonValue,demSettings);

        if (!err.empty()){
            printf("parse error: %s\n",err.c_str());
        }

        if (!jsonValue.is<picojson::object>()){
            printf("root is not object\n");
        }


        /* == read initial files == */
        picojson::object obj = jsonValue.get<picojson::object>();
        picojson::object walls = obj["walls"].get<picojson::object>();
        picojson::array files=walls["filepaths"].get<picojson::array>();

        for (size_t i=0; i<files.size(); i++){

            std::string filePath =files[i].get<std::string>();

            StlMeshLoader loader;

            if(loader.loadASCII(filePath)){
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
    }


    /* == make plane Renderer == */
    PlaneRenderer plane;


    /* ======= main render loop =========== */
    while (!glfwWindowShouldClose(window)) {
        gui.currentFrameTime = static_cast<float>(glfwGetTime());
        camController.updateTime(gui.currentFrameTime);
        camController.processKeyboard(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gui.beginFrame();


        int maxFrame = getMaxFrame(allFrameFiles);


        /* ============= GUI buttons ============== */
        gui.drawSphereControl(camController.camera.Position,maxFrame);
        gui.planeRenderControl(plane);
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
            if(ImGuiFileDialog::Instance()->IsOk()){
                const std::string folderPath = ImGuiFileDialog::Instance()->GetCurrentPath();
                std::vector<std::string> frameFiles = loader.listFrameFiles(folderPath);

                if(!frameFiles.empty()){
                    resultNames.push_back(folderPath);
                    allFrameFiles.push_back(std::move(frameFiles));
                    currentFrames.push_back(FrameData());
                    loadedFrameIndices.push_back(invalidFrameIndex);

                    maxFrame = getMaxFrame(allFrameFiles);
                    if(gui.currentFrame > maxFrame){
                        gui.currentFrame = maxFrame;
                    }
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

        if(gui.requestReload){
            gui.requestReload = false;

            for(std::size_t i = 0; i < resultNames.size(); i++){
                allFrameFiles[i] = loader.listFrameFiles(resultNames[i]);
                currentFrames[i] = FrameData();
                loadedFrameIndices[i] = invalidFrameIndex;
            }

            maxFrame = getMaxFrame(allFrameFiles);
            if(gui.currentFrame > maxFrame){
                gui.currentFrame = maxFrame;
            }
        }
        shader.use();

        camController.setAspectRatio(static_cast<float>(gWindowWidth)/static_cast<float>(gWindowHeight)); //for window size change


        glm::mat4 view = camController.camera.GetViewMatrix();
        glm::mat4 proj = camController.getProjectionMatrix(gui); //set perspective
        shader.setMat4("view", view);
        shader.setMat4("projection", proj);
        shader.setVec3("lightPos", glm::vec3(3.0f, 3.0f, 3.0f));
        shader.setVec3("viewPos", camController.camera.Position);

        //refresh animated frame

        gui.deltaTime = gui.currentFrameTime - gui.lastFrameTime;
        gui.lastFrameTime = gui.currentFrameTime;

        if(gui.isFrameSliderActive){
            timeAccumulator = 0.0f;
        }else{
            timeAccumulator += gui.deltaTime;

            if(timeAccumulator >= frameDuration && !allFrameFiles.empty() && gui.isPlayAnimation){
                timeAccumulator = 0.0f;

                if(maxFrame > 0){
                    gui.currentFrame = (gui.currentFrame + 1) % (maxFrame + 1);
                }else{
                    gui.currentFrame = 0;
                }
            }
        }
        
        loadCurrentFrames(loader, allFrameFiles, gui.currentFrame, currentFrames, loadedFrameIndices);

        /* === render spheres === */

        fastShader.use();
        fastShader.setBool("isOrthographic", gui.isOrthographic);
        fastShader.setMat4("view", view);
        fastShader.setMat4("projection", proj);
        fastShader.setFloat("viewportHeight",(float)gWindowHeight);

        glBindVertexArray(pointVAO);
        glBindBuffer(GL_ARRAY_BUFFER, pointVBO);


        for(std::size_t i = 0; i < currentFrames.size(); i++){
            const FrameData& frame = currentFrames[i];

            if(!frame.isValid || frame.pos.size() != frame.r.size()){
                continue;
            }

            std::vector<float> gpuData(frame.pos.size() * 4);

            for(std::size_t j = 0; j < frame.pos.size(); j++){
                gpuData[j * 4 + 0] = frame.pos[j].x;
                gpuData[j * 4 + 1] = frame.pos[j].y;
                gpuData[j * 4 + 2] = frame.pos[j].z;
                gpuData[j * 4 + 3] = frame.r[j];
            }

            glBufferSubData(
                    GL_ARRAY_BUFFER,
                    0,
                    gpuData.size() * sizeof(float),
                    gpuData.data()
                    );

            Color color;
            color.make_hue(i);

            fastShader.setVec3("objectColor", glm::vec3(color.r, color.g, color.b));
            glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(frame.pos.size()));
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


        /* == display plane ==*/
        plane.render(shader);

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

int getMaxFrame(const std::vector<std::vector<std::string>>& allFrameFiles){
    if(allFrameFiles.empty()){
        return 0;
    }

    std::size_t minimumFrameCount = std::numeric_limits<std::size_t>::max();

    for(std::size_t i = 0; i < allFrameFiles.size(); i++){
        if(allFrameFiles[i].empty()){
            return 0;
        }

        if(allFrameFiles[i].size() < minimumFrameCount){
            minimumFrameCount = allFrameFiles[i].size();
        }
    }

    if(minimumFrameCount == 0 || minimumFrameCount == std::numeric_limits<std::size_t>::max()){
        return 0;
    }

    return static_cast<int>(minimumFrameCount - 1);
}

void loadCurrentFrames(PositionLoader& loader, const std::vector<std::vector<std::string>>& allFrameFiles, int frameIndex, std::vector<FrameData>& currentFrames, std::vector<int>& loadedFrameIndices){
    if(frameIndex < 0){
        return;
    }

    const std::size_t resultCount = allFrameFiles.size();

    if(currentFrames.size() != resultCount){
        currentFrames.resize(resultCount);
    }

    if(loadedFrameIndices.size() != resultCount){
        loadedFrameIndices.resize(resultCount, -1);
    }

    for(std::size_t i = 0; i < resultCount; i++){
        const std::size_t requestedFrame = static_cast<std::size_t>(frameIndex);

        if(requestedFrame >= allFrameFiles[i].size()){
            currentFrames[i] = FrameData();
            loadedFrameIndices[i] = frameIndex;
            continue;
        }

        if(loadedFrameIndices[i] == frameIndex){
            continue;
        }

        currentFrames[i] = loader.load(allFrameFiles[i][requestedFrame]);
        loadedFrameIndices[i] = frameIndex;
    }
}
