#pragma once
#include <glm/glm.hpp>
#include "Constants.h"

namespace Kernel{
    namespace Density{
        
        float W_poly_fast(float r2) {
            if (r2 >= 0 && r2 <= h2) {
                float diff = h2 - r2;
                return 315.0f / (64.0f * pi * h9) * diff * diff * diff;
            }
            return 0.0f;
        }

    }

    namespace Pressure{
    }

    namespace Force{

    }

}