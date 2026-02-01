#pragma once
#include <glm/glm.hpp>
#include "Constants.h"

namespace Kernel{
    namespace Density{
        
        inline float W_poly_fast(float r2) {
            float diff = h2 - r2;
            return 315.0f / (64.0f * pi * h9) * diff * diff * diff;
        }

        inline float wendland_3d(float r2) {
            float r = std::sqrt(r2);
            float q = r / h;
            
            float q_minus_1 = 1.0f - q;
            float q_minus_1_pow4 = q_minus_1 * q_minus_1 * q_minus_1 * q_minus_1;
            
            // 3D Normalization Constant: 21 / (16 * PI * h^3)
            float C = 0.417781356f / (h3);
            
            return C * q_minus_1_pow4 * (1.0f + 4.0f * q);

        }

    }

    namespace Pressure{
    }

    namespace Force{
        
        inline glm::vec3 gradW_poly_fast(const glm::vec3& rij, float r2) {
            if (r2 >= 0 && r2 <= h2) {
                float diff = h2 - r2;
                // Constant: -6 * (315 / 64*pi*h^9)
                float factor = -945.0f / (32.0f * pi * h9) * diff * diff;
                return rij * factor;
            }
            return glm::vec3(0.0f);
        }
        
        inline glm::vec3 grad_wendland_3d(const glm::vec3& r_vec, float r2) {
            float r = std::sqrt(r2);
            float q = r / h;
            if (q >= 1.0f || r <= 1e-9f) 
                return glm::vec3(0.0f);

            // 1. Normalization constant for 3D: 21 / (16 * PI * h^3)
            const float alpha = 0.417781356f / (h * h * h);
                        
            float one_minus_q = 1.0f - q;
            float factor = -alpha * (20.0f / (h * h)) * (one_minus_q * one_minus_q * one_minus_q);
            
            return r_vec * factor;
        }
    }

    namespace Viscosity{
        float laplacianW_viscosity(float r2) {
            float r = sqrt(r2);
            return 45.0f / (pi* h6) * (h - r); // TODO: remove sqrt
        }
    }

}