#include "Scene1.h"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/ext/vector_common.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/trigonometric.hpp"
#include <imgui.h>

// DUMMY IMPLEMENTATIONS



void Scene1::init(){
    rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    running = false;
    sceme = 1;
    std::vector<glm::vec3> positions;

    // for random positions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f); 


    for (int i = 0; i < 10000; ++i) {
        positions.emplace_back(dist(gen), dist(gen), float(i)*0.001);
    }

    for (int i = 0; i < 10000; ++i) {
        positions.emplace_back(dist(gen), 6.0  + dist(gen), float(i)*0.001);
    }

    for (int i = 0; i < 10000; ++i) {
        positions.emplace_back(3.0 + dist(gen), 3.0 + dist(gen), float(i)*0.001);
    }
    fluidSim.init(positions);
}

glm::vec4 Scene1::getColor(glm::vec3 vel){
    glm::vec4 res = glm::vec4(0.4f, 0.4f, 1.0f, 1.0f);
    switch (sceme) {
        case 1:
            return res;
            break;
        case 2:
            glm::vec4 slowest, fastest;
            slowest = res;
            fastest = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            float maxVel = 20.0f;
            float relVel = vel.length() / maxVel;
            relVel <= 1 ? res = (slowest * relVel + (1- relVel) * fastest) : res = fastest;
            return res;
            break;
    }
    return res;
}


void Scene1::onDraw(Renderer& renderer){
    glm::vec4 color{0.4f, 0.4f, 1.0f, 1.0f};
    glm::quat orientation{0.0f, 0.0f, 0.0f, 1.0f};
    glm::vec3 size{0.2f};

    for (auto& cube : fluidSim.particles) {
        glm::vec3 rotatet_pos = glm::rotate(rotation, cube.pos);
        renderer.drawCube(rotatet_pos, orientation, size, getColor(cube.vel));
    }
}

void Scene1::onGUI(){
    ImGui::SliderFloat("Time step", &fluidSim.dt, 0.002f, 0.04f);
    if(ImGui::Button(running ? "Pause" : "Start")){
        running = !running;
    }
    if(ImGui::Button("Reset Rotation")){
        rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }
    ImGui::SliderInt("Color Sceme", &sceme, 1, 3);
    //Keyboard controls for rotation
    if(ImGui::IsKeyPressed(ImGuiKey_LeftArrow)){
        //Rotation to the left   
        glm::quat incRot = glm::angleAxis(glm::radians(-5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        rotation = glm::normalize(incRot * rotation);
    }
    else if(ImGui::IsKeyPressed(ImGuiKey_RightArrow)){
        //Rotation to the right
        glm::quat incRot = glm::angleAxis(glm::radians(5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        rotation = glm::normalize(incRot * rotation);
    }
    else if(ImGui::IsKeyPressed(ImGuiKey_UpArrow)){
        //Rotation Upwards
        glm::quat incRot = glm::angleAxis(glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rotation = glm::normalize(incRot * rotation);
    }
    else if(ImGui::IsKeyPressed(ImGuiKey_DownArrow)){
        //Rotation downwards
        glm::quat incRot = glm::angleAxis(glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rotation = glm::normalize(incRot * rotation);
    }
};

void Scene1::simulateStep(){
    if(running){
        fluidSim.simulateStep(rotation);
    }
};