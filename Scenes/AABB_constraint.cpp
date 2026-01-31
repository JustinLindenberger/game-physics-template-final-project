
#include "AABB_constraint.h"
    
AABBConstraint::AABBConstraint(
    const glm::vec3& center,
    const glm::vec3& scale) noexcept :
    _center(center),
    _scale(scale)
{
    // Creates 6 cubes as floor, walls, and ceiling of a boundary box, the dimensions of which can be definied flexibly.
    glm::vec3 cmXMinus{center.x - scale.x * 2.5f, center.y, center.z};
    glm::vec3 cmXPlus{center.x + scale.x * 2.5f, center.y, center.z};
    glm::vec3 cmYMinus{center.x, center.y - scale.y * 2.5f, center.z};
    glm::vec3 cmYPlus{center.x, center.y + scale.y * 2.5f, center.z};
    glm::vec3 cmZMinus{center.x, center.y, center.z - scale.z * 2.5f};
    glm::vec3 cmZPlus{center.x, center.y, center.z + scale.z * 2.5f};
    glm::quat orientation(1.0f, 0.0f, 0.0f, 0.0f); 
    glm::vec3 linVel{0};
    glm::vec3 angVel{0};
    float mass = 100000000.0f;
    glm::vec3 scaleBox{scale.x * 4.f, scale.y * 4.f, scale.z * 4.f};

    _walls.push_back(Cuboid(nullptr, mass, cmXMinus, orientation, linVel, angVel, scaleBox, true));
    _walls.push_back(Cuboid(nullptr, mass, cmXPlus, orientation, linVel, angVel, scaleBox, true));
    _walls.push_back(Cuboid(nullptr, mass, cmYMinus, orientation, linVel, angVel, scaleBox, true));
    _walls.push_back(Cuboid(nullptr, mass, cmYPlus, orientation, linVel, angVel, scaleBox, true));
    _walls.push_back(Cuboid(nullptr, mass, cmZMinus, orientation, linVel, angVel, scaleBox, true));
    _walls.push_back(Cuboid(nullptr, mass, cmZPlus, orientation, linVel, angVel, scaleBox));
}

void AABBConstraint::ApplyTo(Cuboid& cuboid) noexcept
{
    for (Cuboid& wall : _walls)
        wall.Collide(cuboid);
}

void AABBConstraint::Draw(Renderer& renderer) const noexcept
{ 
    renderer.drawWireCube(_center, _scale, glm::vec4(1.0f));
}

