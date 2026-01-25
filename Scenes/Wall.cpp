
#include "Wall.h"

void Wall::bound(ParticlePool& pool) const noexcept
{
    const glm::vec3 half = 0.5f * m_scale + glm::vec3(ParticlePool::PARTICLE_RADIUS);

    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < pool.size; ++i)
    {
        glm::vec3 pos = pool.positions[i] - m_centre;
        glm::vec3 vel = pool.velocities[i];

        const float ax = (pos.x < 0) ? -pos.x : pos.x;
        if (ax < half.x)
        {
            const float ay = (pos.y < 0) ? -pos.y : pos.y;
            if (ay < half.y)
            {
                const float az = (pos.z < 0) ? -pos.z : pos.z;
                if (az < half.z)
                {
                    const float px = half.x - ax;
                    const float py = half.y - ay;
                    const float pz = half.z - az;

                    if (px < py && px < pz)
                    {
                        pos.x = (pos.x > 0 ? half.x : -half.x);
                        vel.x *= -m_damping;
                    }
                    else if (py < pz)
                    {
                        pos.y = (pos.y > 0 ? half.y : -half.y);
                        vel.y *= -m_damping;
                    }
                    else
                    {
                        pos.z = (pos.z > 0 ? half.z : -half.z);
                        vel.z *= -m_damping;
                    }
                }
            }
        }

        pool.positions[i]  = m_centre + pos;
        pool.velocities[i] = vel;
    }
}

void Wall::draw(Renderer& renderer) const noexcept
{
    renderer.drawWireCube(m_centre, m_scale, COLOUR);
}

