
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class LinearKernel final
{
public:
    static constexpr float RADIUS = 0.3f;

private:
    static constexpr float INV_VOLUME = 3.0f / (glm::pi<float>() * RADIUS * RADIUS * RADIUS * RADIUS);

public:
    static constexpr float ZERO_DISTANCE_W = INV_VOLUME * RADIUS;
    static constexpr float ZERO_DISTANCE_W_DRV = -INV_VOLUME;
    static constexpr float ZERO_DISTANCE_W_DRV2 = 0.0f;
    static constexpr float ZERO_DISTANCE_W_LAPLACIAN = -INFINITY;

    static inline float W(float distance) noexcept 
    {
        const float factor = RADIUS - distance;
        return INV_VOLUME * factor;   
    }

    static inline float WDrv(float distance) noexcept 
    {
        return -INV_VOLUME;
    }

    static inline float WDrv2(float distance) noexcept
    {
        return 0.0f;
    }

    static inline float WLaplacian(float distance) noexcept
    {
        return -2.0f * INV_VOLUME / distance;
    }
};

class CubeKernel final
{
public:
    static constexpr float RADIUS = 0.3f;

private:
    static constexpr float INV_VOLUME = 15.0f / (glm::pi<float>() * RADIUS * RADIUS * RADIUS * RADIUS * RADIUS * RADIUS);
    
public:
    static constexpr float ZERO_DISTANCE_W = INV_VOLUME * RADIUS * RADIUS * RADIUS;
    static constexpr float ZERO_DISTANCE_W_DRV = -3.0f * INV_VOLUME * RADIUS * RADIUS;
    static constexpr float ZERO_DISTANCE_W_DRV2 = 6.0f * INV_VOLUME * RADIUS;
    static constexpr float ZERO_DISTANCE_W_LAPLACIAN = -INFINITY;

    static inline float W(float distance) noexcept 
    {
        const float factor = RADIUS - distance;
        return INV_VOLUME * factor * factor * factor;
    }

    static inline float WDrv(float distance) noexcept 
    {
        const float factor = RADIUS - distance;
        return -3.0f * INV_VOLUME * factor * factor;
    }

    static inline float WDrv2(float distance) noexcept
    {
        const float factor = RADIUS - distance;
        return 6.0f * INV_VOLUME * factor;
    }

    static inline float WLaplacian(float distance) noexcept
    {
        const float factor = RADIUS - distance;
        return 6.0f * INV_VOLUME * factor * (1.0f - factor / distance);
    }
};

class Poly6Kernel final
{
public:
    static constexpr float RADIUS = 0.3f;

private:
    static constexpr float INV_VOLUME = 315.0f / (64.0f * glm::pi<float>() * RADIUS * RADIUS * RADIUS * RADIUS * RADIUS * RADIUS * RADIUS * RADIUS * RADIUS);

public:
    static constexpr float ZERO_DISTANCE_W = INV_VOLUME * RADIUS * RADIUS * RADIUS * RADIUS * RADIUS * RADIUS;
    static constexpr float ZERO_DISTANCE_W_DRV = 0.0f;
    static constexpr float ZERO_DISTANCE_W_DRV2 = -6.0f * INV_VOLUME * RADIUS * RADIUS * RADIUS * RADIUS;
    static constexpr float ZERO_DISTANCE_W_LAPLACIAN = -18.0f * INV_VOLUME * RADIUS * RADIUS * RADIUS * RADIUS;

    static inline float W(float distance) noexcept 
    {
        const float factor = RADIUS * RADIUS - distance * distance;
        return INV_VOLUME * factor * factor * factor;
    }

    static inline float WDrv(float distance) noexcept 
    {
        const float factor = RADIUS * RADIUS - distance * distance;
        return -6.0f * INV_VOLUME * distance * factor * factor;
    }

    static inline float WDrv2(float distance) noexcept
    {
        const float distance2 = distance * distance;
        const float factor = RADIUS * RADIUS - distance2;
        return -6.0f * INV_VOLUME * factor * (RADIUS * RADIUS - 5.0f * distance2);
    }

    static inline float WLaplacian(float distance) noexcept
    {
        const float distance2 = distance * distance;
        const float factor = RADIUS * RADIUS - distance2;
        return -6.0f * INV_VOLUME * factor * (3.0f * RADIUS * RADIUS - 7.0f * distance2);
    }
};

class WendlandC2Kernel final
{
public:
    static constexpr float RADIUS = 0.3f;

private:
    static constexpr float INV_VOLUME = 21.0f / (2.0f * glm::pi<float>() * RADIUS * RADIUS * RADIUS * RADIUS * RADIUS * RADIUS * RADIUS * RADIUS);

public:
    static constexpr float ZERO_DISTANCE_W = INV_VOLUME * RADIUS * RADIUS * RADIUS * RADIUS * RADIUS;
    static constexpr float ZERO_DISTANCE_W_DRV = 0.0f;
    static constexpr float ZERO_DISTANCE_W_DRV2 = -20.0f * INV_VOLUME * RADIUS * RADIUS * RADIUS;
    static constexpr float ZERO_DISTANCE_W_LAPLACIAN = -60.0f * INV_VOLUME * RADIUS * RADIUS * RADIUS;

    static inline float W(float distance) noexcept 
    {
        const float factor = RADIUS - distance;
        const float factor2 = factor * factor;
        const float factor4 = factor2 * factor2;
        return INV_VOLUME * factor4 * (RADIUS + 4.0f * distance);
    }

    static inline float WDrv(float distance) noexcept 
    {
        const float factor = RADIUS - distance;
        return -20.0f * INV_VOLUME * distance * factor * factor * factor;
    }

    static inline float WDrv2(float distance) noexcept
    {
        const float factor = RADIUS - distance;
        return -20.0f * INV_VOLUME * factor * factor * (RADIUS - 4.0f * distance);
    }

    static inline float WLaplacian(float distance) noexcept
    {
        const float factor = RADIUS - distance;
        return -60.0f * INV_VOLUME * factor * factor * (RADIUS - 2.0f * distance);
    }
};
