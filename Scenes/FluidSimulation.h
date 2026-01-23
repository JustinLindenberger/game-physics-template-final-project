#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "Grid.h"
#include "Constants.h"
#include "Particle.h"




class FluidSimulation{

public:
    void init(std::vector<glm::vec3>& positions);
    void simulateStep();

    std::vector<Particle> particles;

private:
    void reset();
    void insertParticelsIntoGrid();
    void densityCalculations();
    void pressureFromDensity();
    void forcesFromPressure();
    void velFromForces();

    Grid grid;

    // SPH constants (TODO: remove/rename/edit)
    const float mass = 1.0f;       // Particle mass

};