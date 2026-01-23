#include "Scene1.h"


// DUMMY IMPLEMENTATIONS



void Scene1::init(){
    std::vector<glm::vec3> positions;

    // for random positions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-5.0f, 5.0f); 


    for (int i = 0; i < 1000; ++i) {
        positions.emplace_back(dist(gen), dist(gen), dist(gen));
    }
    fluidSim.init(positions);
}




void Scene1::onDraw(Renderer& renderer){
    glm::vec4 color{0.4f, 0.4f, 1.0f, 0.6f};
    glm::quat orientation{0.0f, 0.0f, 0.0f, 1.0f};
    glm::vec3 size{0.2f};

    for (auto& cube : fluidSim.particles) {
        renderer.drawCube(cube.pos, orientation, size, color);
    }
}

void Scene1::onGUI(){

};

void Scene1::simulateStep(){
    fluidSim.simulateStep();
};