
#include "Cuboid.h"

Cuboid::Cuboid(
    std::vector<Particle>* particles,
    float mass,
    const glm::vec3& positionOfCentreOfMass, 
    const glm::quat& orientation,
    const glm::vec3& linearVelocity,
    const glm::vec3& angularVelocity,
    const glm::vec3& scale,
    bool isStatic) noexcept :
        _mass{mass},
        _inverseMass{1.0f/mass},
        _positionOfCentreOfMass{positionOfCentreOfMass},
        _orientation{glm::normalize(orientation)},
        _linearVelocity{linearVelocity},
        _angularVelocity{angularVelocity},
        _scale{scale},
        _particles{particles},
        _isStatic{isStatic}
{
    if (particles) {     
        // Figure out how many boundary particles need to be on the surface as well as their positions. Put those particles into the FluidSimulations particle list
        int nx = (_scale.x / (spacing * h)) + 1;
        int ny = (_scale.y / (spacing * h)) + 1;
        int nz = (_scale.z / (spacing * h)) + 1;

        float dx = _scale.x / (nx - 1);
        float dy = _scale.y / (ny - 1);
        float dz = _scale.z / (nz - 1);

        _localParticlePositions.reserve(nx * ny * nz);

        for (int ix = 0; ix < nx; ++ix) {
            for (int iy = 0; iy < ny; ++iy) {
                for (int iz = 0; iz < nz; ++iz) {
                    bool onSurface = (ix <= 1 || ix >= nx - 2 ||
                                    iy <= 1 || iy >= ny - 2 ||
                                    iz <= 1 || iz >= nz - 2);
                    if (!onSurface) continue;

                    glm::vec3 lp{(-0.5f * _scale.x) + ix * dx, (-0.5f * _scale.y) + iy * dy, (-0.5f * _scale.z) + iz * dz};
                    _localParticlePositions.push_back(lp);
                }
            }
        }

        _start = _particles->size();
        _particles->reserve(_particles->size() + _localParticlePositions.size());
        for (auto& lp : _localParticlePositions)
            _particles->emplace_back(_positionOfCentreOfMass + _orientation * lp, true);
        _end = (int)_particles->size();
    }

    // Inverse Base Inertia Tensor, based on the wikipedia article, assuming uniform mass distribution throughout the cube.
    if (_isStatic) {
        _inverseMass = 0.0f;
        _inverseBaseInertiaTensor = glm::mat3(0.0f);
        _inverseInertiaTensor = glm::mat3(0.0f);
        _angularMomentum = glm::vec3(0.0f);
        _linearVelocity = glm::vec3(0.0f);
        _angularVelocity = glm::vec3(0.0f);
    } else {
        _inverseBaseInertiaTensor = glm::mat3{0};
        _inverseBaseInertiaTensor[0][0] = 1.f/((1.f/12.f) * mass * (scale.y * scale.y + scale.z * scale.z));
        _inverseBaseInertiaTensor[1][1] = 1.f/((1.f/12.f) * mass * (scale.x * scale.x + scale.z * scale.z));
        _inverseBaseInertiaTensor[2][2] = 1.f/((1.f/12.f) * mass * (scale.x * scale.x + scale.y * scale.y));
        _inverseInertiaTensor = glm::mat3_cast(orientation) * _inverseBaseInertiaTensor * glm::transpose(glm::mat3_cast(orientation));
        _angularMomentum = glm::inverse(_inverseInertiaTensor) * angularVelocity;

    }
}

// Determine boundary particles new position based on where the Cuboids position and orientation, also reset their forces for the next simulation round.
void Cuboid::updateBoundaryParticles() const noexcept {
    for (int i = 0; i < _localParticlePositions.size(); i++) {
        (*_particles)[_start + i].pos = _positionOfCentreOfMass + _orientation * _localParticlePositions[i];
        (*_particles)[_start + i].force = glm::vec3(0.0f); 
    }
}

// get velocity of point respective of the cube's center of mass
glm::vec3 Cuboid::GetWorldSpaceVelocityAt(const glm::vec3& leverArm) const noexcept {
    return _linearVelocity + glm::cross(_angularVelocity, leverArm);
}

// Model matrix necessary for the collision detection function.
glm::mat4 Cuboid::GetModelMatrix() const noexcept {
    glm::mat4 rotationMatrix = glm::toMat4(_orientation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1), _scale);
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1), _positionOfCentreOfMass);
    return translationMatrix * rotationMatrix * scaleMatrix;
}

void Cuboid::Collide(Cuboid& other) noexcept {
    glm::mat4 obj2World_A = GetModelMatrix();
    glm::mat4 obj2World_B = other.GetModelMatrix();

    CollisionInfo cInfo = collisionTools::checkCollisionSAT(obj2World_A, obj2World_B);
    if (!cInfo.isColliding) return;

    glm::vec3 posA = cInfo.collisionPointWorld - _positionOfCentreOfMass;
    glm::vec3 posB = cInfo.collisionPointWorld - other._positionOfCentreOfMass;
    // get velocities of the two collision points respective to the center of mass of their cube.
    glm::vec3 vA = GetWorldSpaceVelocityAt(posA);
    glm::vec3 vB = other.GetWorldSpaceVelocityAt(posB);
    float vRelAlongNormal = glm::dot(vA - vB, cInfo.normalWorld);
    // If the dot product of the relative velocity of the collision points and the collison's normal vector is greater than zero the bodies are separating.
    if (vRelAlongNormal > 0.0f) return;

    //All of this is just the formula for calculating the impulse.
    glm::vec3 angularImpulsTermA = glm::cross(_inverseInertiaTensor * glm::cross(posA, cInfo.normalWorld), posA);
    glm::vec3 angularImpulsTermB = glm::cross(other._inverseInertiaTensor * glm::cross(posB, cInfo.normalWorld), posB);

    float denom = _inverseMass + other._inverseMass + glm::dot(angularImpulsTermA + angularImpulsTermB, cInfo.normalWorld);

    if (denom <= 1e-9f) return; // prevent divide by zero

    float j = -(1.0f + glm::min(_restitution, other._restitution)) * vRelAlongNormal / denom;
    glm::vec3 impulse = j * cInfo.normalWorld;

    // Update velocity and momentum based on the impulse.
    if (!_isStatic) {
        _linearVelocity += impulse * _inverseMass;
        _angularMomentum += glm::cross(posA, impulse);
        _angularVelocity = _inverseInertiaTensor * _angularMomentum;
        _positionOfCentreOfMass += cInfo.normalWorld * cInfo.depth * (other._mass / (_mass + other._mass)); // This is a dirty fix to avoid the Cuboids from falling through the floor.
    }
    if (!other._isStatic) {
        other._linearVelocity -= impulse * other._inverseMass;
        other._angularMomentum -= glm::cross(posB, impulse);
        other._angularVelocity = other._inverseInertiaTensor * other._angularMomentum;
        other._positionOfCentreOfMass -= cInfo.normalWorld * cInfo.depth * (_mass / (_mass + other._mass)); // This is a dirty fix to avoid the Cuboids from falling through the floor.
    }
}

// Apply forces to the Cuboid based on the forces exerted on the boundary positions.
void Cuboid::integrateFromBoundaryForces(float dt) noexcept {
    glm::vec3 force(0.0f);
    glm::vec3 torque(0.0f);
    for (int i = _start; i < _end; ++i) {
        const glm::vec3 f = (*_particles)[i].force;
        force += f;
        torque += glm::cross((*_particles)[i].pos - _positionOfCentreOfMass, f);
    }

    _linearVelocity += ((force * _inverseMass) + gravity) * dt;
    _positionOfCentreOfMass += _linearVelocity * dt;

    glm::mat3 R = glm::mat3_cast(_orientation);
    _inverseInertiaTensor = R * _inverseBaseInertiaTensor
                            * glm::transpose(R);

    _angularMomentum += dt * torque;

    _angularVelocity = _inverseInertiaTensor * _angularMomentum;

    _orientation += dt * 0.5f * glm::quat(0.0f, _angularVelocity) * _orientation;
    _orientation = glm::normalize(_orientation);
}