
#include "ParticlePool.h"

glm::vec4 ParticlePool::getColour(size_t index) const noexcept 
{
    constexpr float blue2  = BLUE_SPEED  * BLUE_SPEED;
    constexpr float green2 = GREEN_SPEED * GREEN_SPEED;
    constexpr float red2   = RED_SPEED   * RED_SPEED;

    const float v2 = glm::dot(velocities[index], velocities[index]);
    const float t  = glm::clamp((v2 - blue2) / (red2 - blue2), 0.0f, 1.0f);

    const float red   = glm::clamp((t - 0.5f) * 2.0f, 0.0f, 1.0f);
    const float blue  = glm::clamp((0.5f - t) * 2.0f, 0.0f, 1.0f);
    const float green = 1.0f - red - blue;

    return glm::vec4(red, green, blue, 0.6f);
}

void ParticlePool::resize(size_t n) noexcept
{
    positions.resize(n);
    velocities.resize(n);
    forces.resize(n);
    densities.resize(n);
    pressures.resize(n);
    size = n;
}

void ParticlePool::draw(Renderer& renderer) const noexcept 
{
    # pragma omp parallel for schedule(static)
    for (size_t i = 0; i < size; ++i)
        renderer.drawSphere(positions[i], PARTICLE_RADIUS, DEFAULT_COLOUR);
}

void ParticlePool::drawWithSpeed(Renderer& renderer) const noexcept 
{
    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < size; ++i)
        renderer.drawSphere(positions[i], PARTICLE_RADIUS, getColour(i));
}


