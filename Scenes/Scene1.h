#pragma once
#include "Scene.h"
#include "FluidSimulation.h"
#include "glm/fwd.hpp"

#include <random>

class Scene1 : public Scene
{
    public:
        void init();
        void onDraw(Renderer& renderer);
        void onGUI();
        void simulateStep();
    
    private:
        FluidSimulation fluidSim;
        bool running;
        int sceme;
        glm::quat rotation;
        glm::vec4 getColor(const glm::vec3& vel, float density);
        glm::vec4 slowestColor = glm::vec4(0.4f, 0.4f, 1.0f, 1.0f); // blue
        glm::vec4 fastestColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // white
};