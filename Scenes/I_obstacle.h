
#pragma once

#include "ParticlePool.h"

class IObstacle
{
protected:
    glm::vec3 m_centre;
    glm::vec3 m_scale;
    float m_damping;

    IObstacle() = default;

    IObstacle(const glm::vec3& centre, const glm::vec3& scale, float damping) noexcept
        : m_centre(centre), m_scale(scale), m_damping(damping) { }

public:
    virtual void bound(ParticlePool& pool) const noexcept = 0;
    virtual void draw(Renderer& renderer) const noexcept = 0;
};

