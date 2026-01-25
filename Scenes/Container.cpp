
#include "Container.h"

void Container::bound(ParticlePool& pool) const noexcept
{
    const glm::vec3 half = 0.5f * m_scale - glm::vec3(ParticlePool::PARTICLE_RADIUS);

    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < pool.size; ++i)
    {
        glm::vec3 pos = pool.positions[i] - m_centre;
        glm::vec3 vel = pool.velocities[i];

        if (pos.x >  half.x) { pos.x =  half.x; vel.x *= -m_damping; }
        if (pos.x < -half.x) { pos.x = -half.x; vel.x *= -m_damping; }

        if (pos.y >  half.y) { pos.y =  half.y; vel.y *= -m_damping; }
        if (pos.y < -half.y) { pos.y = -half.y; vel.y *= -m_damping; }

        if (pos.z >  half.z) { pos.z =  half.z; vel.z *= -m_damping; }
        if (pos.z < -half.z) { pos.z = -half.z; vel.z *= -m_damping; }

        pool.positions[i]  = m_centre + pos;
        pool.velocities[i] = vel;
    }
}

void Container::draw(Renderer& renderer) const noexcept
{
   renderer.drawWireCube(m_centre, m_scale, COLOUR); 
}

