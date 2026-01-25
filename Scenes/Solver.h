
#pragma once

#include <string>
#include "Timers.h"
#include "Kernels.h"
#include "ParticlePool.h"
#include "UniformGrid.h"

template<class Kernel>
class Solver 
{
private:
    static constexpr glm::vec3 GRAVITY = glm::vec3(0.0f, 0.0f, -1.0f);

    float m_restDensity;
    float m_pressureStiffness;
    float m_pressureGamma;
    float m_viscosity;

    std::vector<float> m_pressureInfluences;

    std::unordered_map<std::string, ProfileData> m_profileLogs;

    void updateDensities(ParticlePool& pool, const UniformGrid& grid) const noexcept
    {
        #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < pool.size; ++i) 
        {
            pool.densities[i] = ParticlePool::PARTICLE_MASS * Kernel::ZERO_DISTANCE_W;
        }

        // We cannot parallelise due to the race condition
        // #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < pool.size; ++i) 
        {
            grid.forEachNeighbor(pool, i, [&](size_t j) {
                if (i >= j) return;
                const glm::vec3 relativePos = pool.positions[i] - pool.positions[j];
                const float distance2 = glm::dot(relativePos, relativePos);
                if (distance2 >= Kernel::RADIUS * Kernel::RADIUS) return;
                const float distance = glm::sqrt(distance2);
                const float influence = ParticlePool::PARTICLE_MASS * Kernel::W(distance);
                pool.densities[i] += influence;
                pool.densities[j] += influence;
            });
        }

        // for (size_t i = 0; i < pool.size; ++i)
        //     std::cout << "Density " << i << ": " << pool.densities[i] << std::endl;
    }

    void updatePressures(ParticlePool& pool) const noexcept
    {
        #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < pool.size; ++i) 
        {
            const float ratio = pool.densities[i] / m_restDensity;
            const float pressure = m_pressureStiffness * (glm::pow(ratio, m_pressureGamma) - 1.0f);
            pool.pressures[i] = pressure;
        }

        // for (size_t i = 0; i < pool.size; ++i)
        //     std::cout << "Pressure " << i << ": " << pool.pressures[i] << std::endl;
    }

    void updateForces(ParticlePool& pool, const UniformGrid& grid) noexcept
    {
        #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < pool.size; ++i) 
        {
            pool.forces[i] = glm::vec3(0.0f);
        }

        #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < pool.size; ++i) 
        {
            m_pressureInfluences[i] = pool.pressures[i] / (pool.densities[i] * pool.densities[i]);
        }

        // We cannot parallelise due to the race condition
        // #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < pool.size; ++i) 
        {
            grid.forEachNeighbor(pool, i, [&](size_t j) {
                if (i >= j) return;
                const glm::vec3 relativePos = pool.positions[i] - pool.positions[j];
                const float distance2 = glm::dot(relativePos, relativePos);
                if (distance2 < 1e-5f || distance2 >= Kernel::RADIUS * Kernel::RADIUS) return;
                const float distance = glm::sqrt(distance2);
                const glm::vec3 direction = relativePos / distance;
                const glm::vec3 gradient = direction * Kernel::WDrv(distance);
                const glm::vec3 pressureInfluence = -ParticlePool::PARTICLE_MASS * (m_pressureInfluences[i] + m_pressureInfluences[j]) * gradient;

                const glm::vec3 relativeVel = pool.velocities[j] - pool.velocities[i];
                const glm::vec3 viscosityInfluence = ParticlePool::PARTICLE_MASS * 2.0f * m_viscosity / 
                    (pool.densities[i] * pool.densities[j]) * relativeVel * glm::dot(relativePos, gradient) / (distance2 + glm::epsilon<float>());

                const glm::vec3 combinedInfluence = pressureInfluence + viscosityInfluence;

                pool.forces[i] += combinedInfluence;
                pool.forces[j] -= combinedInfluence;
            });
        }

        // for (size_t i = 0; i < pool.size; ++i)
        //     std::cout << "Force " << i << ": " << pool.forces[i].x << ", " << pool.forces[i].y << ", " << pool.forces[i].z << std::endl;
    }

    void integrateSemiImplicitEuler(ParticlePool& pool, float stepTime) const noexcept
    {
        #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < pool.size; ++i) 
        {
            const glm::vec3 acceleration = GRAVITY + pool.forces[i] / ParticlePool::PARTICLE_MASS;
            pool.velocities[i] += acceleration * stepTime;
            pool.positions[i] += pool.velocities[i] * stepTime;
        }

        // for (size_t i = 0; i < pool.size; ++i)
        //     std::cout << "Velocity " << i << ": " << pool.velocities[i].x << ", " << pool.velocities[i].y << ", " << pool.velocities[i].z << std::endl;
        // for (size_t i = 0; i < pool.size; ++i)
        //     std::cout << "Position " << i << ": " << pool.positions[i].x << ", " << pool.positions[i].y << ", " << pool.positions[i].z << std::endl;
    }

public:
    Solver() = default;

    Solver(
        size_t numParticles, 
        float restDensity = 50.0f, 
        float pressureStiffness = 14.3f, 
        float pressureGamma = 3.0f, 
        float viscosity = 0.001f) : 
        m_restDensity(restDensity), 
        m_pressureStiffness(pressureStiffness), 
        m_pressureGamma(pressureGamma),
        m_viscosity(viscosity)
    {
        m_pressureInfluences.resize(numParticles);
    }

    void step(ParticlePool& pool, const UniformGrid& grid, float stepTime) noexcept
    {
        { Timer t("Update Densities",    m_profileLogs); updateDensities(pool, grid);                }
        { Timer t("Update Pressures",    m_profileLogs); updatePressures(pool);                      }
        { Timer t("Update Forces",       m_profileLogs); updateForces(pool, grid);                   }
        { Timer t("Integrate Particles", m_profileLogs); integrateSemiImplicitEuler(pool, stepTime); }
    }
};

