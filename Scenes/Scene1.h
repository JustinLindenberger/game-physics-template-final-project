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
        glm::vec4 getColor(Particle p);
};