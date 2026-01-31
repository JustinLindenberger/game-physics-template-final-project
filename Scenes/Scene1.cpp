#include "Scene1.h"
#include <imgui.h>

// DUMMY IMPLEMENTATIONS



void Scene1::init(){
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

void Scene1::onDraw(Renderer& renderer){
    glm::vec4 color{0.4f, 0.4f, 1.0f, 1.0f};
    glm::quat orientation{0.0f, 0.0f, 0.0f, 1.0f};
    glm::vec3 size{0.2f};

    for (auto& cube : fluidSim.particles) {
        // renderer.drawCube(cube.pos, orientation, size, color);
        if(cube.isRigid) { 
            renderer.drawCube(cube.pos, orientation, size, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}); }
        else {
            renderer.drawCube(cube.pos, orientation, size, color);
        }
    }
    // for (auto& rigidBody : fluidSim.cubes) {
    //     renderer.drawCube(rigidBody._positionOfCentreOfMass, rigidBody._orientation, rigidBody._scale, glm::vec4(1.0f));
    // }
    fluidSim.constraint.Draw(renderer);
}

void Scene1::onGUI(){
    ImGui::SliderFloat("Time step", &fluidSim.dt, 0.002f, 0.04f);
};

void Scene1::simulateStep(){
    fluidSim.simulateStep();
};