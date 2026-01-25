
#pragma once

#include "Renderer.h"

struct ParticlePool final
{
private:
    static constexpr glm::vec4 DEFAULT_COLOUR = glm::vec4(0.4f, 0.4f, 1.0f, 0.6f);

    static constexpr float BLUE_SPEED  = 0.0f;
    static constexpr float GREEN_SPEED = 2.0f;
    static constexpr float RED_SPEED   = 4.0f;

    glm::vec4 getColour(size_t index) const noexcept;

public:
    static constexpr float PARTICLE_RADIUS = 0.1f;
    static constexpr float PARTICLE_MASS   = 1.0f;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocities;
    std::vector<glm::vec3> forces;
    std::vector<float> densities;
    std::vector<float> pressures;

    size_t size = 0;

    void resize(size_t n) noexcept;
    void draw(Renderer& renderer) const noexcept;
    void drawWithSpeed(Renderer& renderer) const noexcept;
};

