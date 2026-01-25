
#pragma once

#include "I_obstacle.h"

class Container final : public IObstacle
{
private:
    static constexpr glm::vec3 COLOUR = glm::vec3(1.0f);

public:
    Container() = default;

    Container(const glm::vec3& centre, const glm::vec3& scale, float damping) noexcept
        : IObstacle(centre, scale, damping) { }

    void bound(ParticlePool& pool) const noexcept override;
    void draw(Renderer& renderer) const noexcept override;
};
