#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Particle{
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 force;

    float density;
    float pressure;

    bool isRigid;

    // Default constructor
    Particle() : pos(0.0f), vel(0.0f), force(0.0f), density(0.0f), pressure(0.0f), isRigid(false) {}

    // Pos-init constuctor
    Particle(glm::vec3 p, bool rigid = false) : pos(p), vel(0.0f), force(0.0f), density(0.0f), pressure(0.0f), isRigid(rigid) {}
};