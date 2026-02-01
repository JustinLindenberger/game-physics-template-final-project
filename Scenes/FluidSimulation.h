#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include "Grid.h"
#include "Timers.h"
#include "Constants.h"
#include "Particle.h"
#include "Cuboid.h"
#include "AABB_constraint.h"
#include "glm/gtc/quaternion.hpp"





class FluidSimulation{

public:
    void init(std::vector<glm::vec3>& positions);
    void simulateStep(glm::quat rotation);

    std::vector<Particle> particles;
    std::vector<Cuboid> cubes;
    AABBConstraint constraint;

    float dt = 0.005;
    float viscosity = 1.0f;

private:
    void reset();
    void insertParticelsIntoGrid();
    void densityCalculations();
    void pressureFromDensity();
    void forcesFromPressure(glm::quat rotation);
    void velFromForces(glm::quat rotation);
    void rigidBodyCollisionAndBoundaries(glm::quat rotation);

    Grid grid;

    std::unordered_map<std::string, ProfileData> profileLogs;

    // SPH constants (TODO: remove/rename/edit)
    const float mass = 1.0f;       // Particle mass
    const float restDensity = 200.0f; // Fluid rest density (kg/m³)
    const float kappa = 1000.0f; // Stiffness constant
    const float restitution = 0.5f;
    const float wallRepulsion = 500.0f;
    // derived values
    const float invRestDensity{1.0f / restDensity};
   

};