#include "Scene1.h"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/ext/vector_common.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/trigonometric.hpp"
#include <imgui.h>

// DUMMY IMPLEMENTATIONS

glm::vec3 gravity = glm::vec3{0.0f, 0.0f, -9.81};

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

    for (int i = 0; i < 3; ++i) {
        Cuboid cube(&fluidSim.particles, 10.0f, glm::vec3(4.0f * dist(gen), 4.0f * dist(gen),  4.5f + 4.0f * dist(gen)));
        fluidSim.cubes.push_back(cube);
    }
    Cuboid cube(&fluidSim.particles, 80.0f, glm::vec3(4.0f * dist(gen), 4.0f * dist(gen),  5.0f + 4.0f * dist(gen)), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    fluidSim.cubes.push_back(cube);
    fluidSim.init(positions);
}

glm::vec4 Scene1::getColor(Particle particle){
    glm::vec4 res = glm::vec4(0.4f, 0.4f, 1.0f, 1.0f);
    constexpr float blue = 0.0f * 0.0f;
    constexpr float green = 2.0f * 2.0f;
    constexpr float red = 4.0f * 4.0f;

    const float v2 = glm::dot(particle.vel, particle.vel);
    const float t = glm::clamp((v2 - blue)/(red - blue), 0.0f, 1.0f);

    const float r = glm::clamp((t - 0.5f) * 2.0f, 0.0f, 1.0f);
    const float b = glm::clamp((0.5f - t) * 2.0f, 0.0f, 1.0f);
    const float g = 1.0f - red - blue;

    return glm::vec4(red, green, blue, 0.6f);
}


void Scene1::onDraw(Renderer& renderer){
    glm::vec4 color{0.4f, 0.4f, 1.0f, 1.0f};
    glm::quat orientation{0.0f, 0.0f, 0.0f, 1.0f};
    glm::vec3 size{0.2f};

    for (auto& cube : fluidSim.particles) {
        glm::vec3 rotatet_pos = glm::rotate(rotation, cube.pos);
        if(cube.isRigid) { 
            renderer.drawCube(rotatet_pos, orientation, size, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}); }
        else {
            renderer.drawCube(rotatet_pos, orientation, size, getColor(cube));
        }
    }
    // for (auto& rigidBody : fluidSim.cubes) {
    //     renderer.drawCube(rigidBody._positionOfCentreOfMass, rigidBody._orientation, rigidBody._scale, glm::vec4(1.0f));
    // }
    fluidSim.constraint.Draw(renderer);
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

    ImGui::SliderFloat("Viscosity", &fluidSim.viscosity, 0.0f, 2.5f);
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

    gravity = glm::rotate(glm::normalize(glm::conjugate(rotation)), glm::vec3{0.0f, 0.0f, -9.81});
};

void Scene1::simulateStep(){
    if(running){
        fluidSim.simulateStep(rotation);
    }
};