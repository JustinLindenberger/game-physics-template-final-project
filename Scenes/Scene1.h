#pragma once
#include "Scene.h"
#include "FluidSimulation.h"

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
};