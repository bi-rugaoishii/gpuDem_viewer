// GuiManager.cpp
#include "GuiManager.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <glm/gtc/type_ptr.hpp>

void GuiManager::init(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void GuiManager::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GuiManager::drawStlControl(const std::vector<std::string>& names)
{
    ImGui::Begin("STL Control");

    if(ImGui::Button("Load STL"))
    {
        openStlDialog = true;
    }

    ImGui::Separator();
    ImGui::Text("Loaded STL:");

    for(size_t i=0;i<names.size();i++)
    {
        ImGui::Text("%s", names[i].c_str());
    }

    ImGui::End();
}

void GuiManager::drawResultControl(const std::vector<std::string>& names)
{
    ImGui::Begin("Draw Result");

    if(ImGui::Button("Load Result"))
    {
        openResultDialog = true;
    }

    ImGui::Separator();
    ImGui::Text("Loaded Result:");

    for(size_t i=0;i<names.size();i++)
    {
        ImGui::Text("%s", names[i].c_str());
    }

    ImGui::End();
}


void GuiManager::planeRenderControl(PlaneRenderer &plane){
    ImGui::Begin("Plane Render Mode");

    ImGui::Checkbox("Show Plane", &plane.visible);

    ImGui::DragFloat3("Plane Position", glm::value_ptr(plane.position), 0.1f);

    ImGui::DragFloat3("Plane Normal", glm::value_ptr(plane.normal), 0.01f);

    ImGui::DragFloat("Plane Size", &plane.size, 0.1f, 0.1f, 1000.0f);
    ImGui::DragFloat("Plane thickness", &plane.thickness, 0.1f, 0.0f, 1.0f);

    ImGui::End();
}

void GuiManager::renderModeControl(){
    ImGui::Begin("Render Mode");

    if(ImGui::Button("Sphere Mode"))
        isFastPointMode = false;

    ImGui::SameLine();

    if(ImGui::Button("Fast Mode"))
        isFastPointMode = true;

    ImGui::End();
}

void GuiManager::drawSphereControl(const glm::vec3& cameraPos, const int maxFrame) {
    ImGui::Begin("Animation Control");


    //scale bars
    /*
       ImGui::SliderFloat("Scale", &scale, 0.03f, 0.1f);
       */

    ImGui::Text("Camera Pos: %.1f %.1f %.1f", cameraPos.x, cameraPos.y, cameraPos.z);

    //frameBars
    ImGui::SliderInt("Frame",&currentFrame, 0, maxFrame);
    isFrameSliderActive = ImGui::IsItemActive();

    // ======================================
    // Frame step buttons
    // ======================================

    // -------- LEFT BUTTON (back) --------
    ImGui::Button("<");

    bool leftClicked = ImGui::IsItemActivated();
    bool leftHolding = ImGui::IsItemActive();

    if(leftClicked)
    {
        currentFrame--;
        if(currentFrame < 0) currentFrame = 0;
        holdTimerLeft = 0.0f;
    }

    if(leftHolding)
    {
        holdTimerLeft += deltaTime;

        if(holdTimerLeft >= holdDelay)
        {
            static float repeatTimerL = 0.0f;
            repeatTimerL += deltaTime;

            if(repeatTimerL >= holdInterval){

                currentFrame--;
                if(currentFrame < 0) currentFrame = 0;
                repeatTimerL = 0.0f;
            }
        }
    }else{
        holdTimerLeft = 0.0f;
    }

    ImGui::SameLine();

    /* ===== Play Button ==== */

    if (ImGui::Button(isPlayAnimation? "Pause":"Play")){
        isPlayAnimation =!isPlayAnimation;
    }

    ImGui::SameLine();

    // -------- RIGHT BUTTON (forward) --------
    ImGui::Button(">");

    bool rightClicked = ImGui::IsItemActivated();
    bool rightHolding = ImGui::IsItemActive();

    if(rightClicked)
    {
        currentFrame++;
        if(currentFrame > maxFrame) currentFrame = maxFrame;
        holdTimerRight = 0.0f;
    }
    if(rightHolding)
    {
        holdTimerRight += deltaTime;

        if(holdTimerRight >= holdDelay)
        {
            static float repeatTimerR = 0.0f;
            repeatTimerR += deltaTime;

            if(repeatTimerR >= holdInterval)
            {
                currentFrame++;
                if(currentFrame > maxFrame) currentFrame = maxFrame;
                repeatTimerR = 0.0f;
            }
        }
    }
    else
    {
        holdTimerRight = 0.0f;
    }

    ImGui::Text("Frame %d / %d", currentFrame, maxFrame);

    /* === reload button == */

    if (ImGui::Button("Reload Frames")){
        requestReload = true;
    }

    /* === is orthographic == */

    ImGui::Checkbox("Orthographic",&this->isOrthographic);

    ImGui::End();
}



void GuiManager::render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiManager::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
