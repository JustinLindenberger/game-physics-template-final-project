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
    fluidSim.init(positions);
}




void Scene1::onDraw(Renderer& renderer){
    glm::vec4 color{0.4f, 0.4f, 1.0f, 1.0f};
    glm::quat orientation{0.0f, 0.0f, 0.0f, 1.0f};
    glm::vec3 size{0.2f};

    for (auto& cube : fluidSim.particles) {
        renderer.drawCube(cube.pos, orientation, size, color);
    }
}

void Scene1::onGUI(){
    ImGui::SliderFloat("Time step", &fluidSim.dt, 0.002f, 0.04f);
};

void Scene1::simulateStep(){
    fluidSim.simulateStep();
};