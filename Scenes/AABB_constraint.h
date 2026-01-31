
#pragma once

#include <vector>
#include "Cuboid.h"
#include "Renderer.h"

// Axis-Aligned Bounding Box Constraint
class AABBConstraint final
{
private:
    glm::vec3 _center;
    glm::vec3 _scale;

    std::vector<Cuboid> _walls;

public:
    AABBConstraint(
        const glm::vec3& centre = glm::vec3(0.0f),
        const glm::vec3& scale = glm::vec3(10.0f)) noexcept;

    void ApplyTo(Cuboid& cuboid) noexcept; 
    void Draw(Renderer& renderer) const noexcept;
};

