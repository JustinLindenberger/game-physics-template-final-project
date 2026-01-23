#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Particle{
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 force;

    float density;
    float pressure;

    // Default constructor
    Particle() : pos(0.0f), vel(0.0f), force(0.0f), density(0.0f), pressure(0.0f) {}

    // Pos-init constuctor
    Particle(glm::vec3 p) : pos(p), vel(0.0f), force(0.0f), density(0.0f), pressure(0.0f) {}
};