#pragma once
#include "util/CollisionDetection.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/string_cast.hpp"
#include "Constants.h"
#include "Particle.h"
#include <vector>

class Cuboid final
{
private:
    float _mass; // m
    float _inverseMass; // 1.0 / m
    glm::vec3 _linearVelocity; // v_cm
    glm::vec3 _angularVelocity; // w
    glm::vec3 _angularMomentum; // L
    glm::mat3 _inverseBaseInertiaTensor; // (I_0)^(-1)
    glm::mat3 _inverseInertiaTensor; // I^(-1)
    int _start = 0;
    int _end = 0;
    std::vector<Particle> *_particles;
    std::vector<glm::vec3> _localParticlePositions;
    const float _restitution = 0.1f;
    bool _isStatic = false;
    const float spacing = 0.5;

public:
    glm::vec3 _positionOfCentreOfMass; // x_cm
    glm::quat _orientation; // q
    glm::vec3 _scale; // How much a cuboid extends in x, y, and z directions

    Cuboid(
        std::vector<Particle>* particles,
        float mass,
        const glm::vec3& positionOfCentreOfMass = glm::vec3{0.0f}, 
        const glm::quat& orientation = glm::quat{1.0f, 0.0f, 0.0f, 0.0f},
        const glm::vec3& linearVelocity = glm::vec3{0.0f},
        const glm::vec3& angularVelocity = glm::vec3{0.0f},
        const glm::vec3& scale = glm::vec3{1.0f, 1.0f, 1.0f},
        bool _isStatic = false) noexcept;

    void updateBoundaryParticles() const noexcept;
    glm::vec3 GetWorldSpaceVelocityAt(const glm::vec3& leverArm) const noexcept; // get velocity of point respective of the cube's center of massnoexcept
    glm::mat4 GetModelMatrix() const noexcept; // Model matrix necessary for the collision detection function.
    void Collide(Cuboid& other) noexcept;
    void integrateFromBoundaryForces(glm::quat rotation, float dt) noexcept;
};

