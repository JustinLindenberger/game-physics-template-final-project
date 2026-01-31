#include "FluidSimulation.h"
#include "Kernels.h"
#include "Scenes/Constants.h"
#include "glm/ext/quaternion_common.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtx/quaternion.hpp"
#include <atomic>
#include <iostream>
#include <execution>
#include <algorithm>


void FluidSimulation::init(std::vector<glm::vec3>& positions){

    particles.reserve(positions.size());

    for (auto& pos : positions) {
        // Create particles with pos filled in and other attributes are set to 0
        particles.emplace_back(pos);
    }
    constraint = AABBConstraint(glm::vec3(0.0f, 0.0f, 10.f), glm::vec3{20.f});
}


void FluidSimulation::simulateStep(glm::quat rotation) {
    { Timer t("1. Reset", profileLogs); reset(); }
    { Timer t("2. Grid Insertion", profileLogs); insertParticelsIntoGrid(); }
    { Timer t("3. Density", profileLogs); densityCalculations(); }
    { Timer t("4. Pressure", profileLogs); pressureFromDensity(); }
    { Timer t("5. Forces", profileLogs); forcesFromPressure(rotation); }
    { Timer t("6. Velocity", profileLogs); velFromForces(rotation); }
    { Timer t("7. Rigid Bodies", profileLogs); rigidBodyCollisionAndBoundaries(rotation);}
}


void FluidSimulation::reset(){
    // reset e.g. densities or the grid
    grid.clear();
}

void FluidSimulation::insertParticelsIntoGrid(){
    // insert particles at their current locaiton into the grid
    for (size_t i = 0; i < particles.size(); ++i)
    {
        grid.insertParticle(particles[i].pos, i);
        
    }
}


void FluidSimulation::densityCalculations(){
    // calculate the density based on the position of the particles

    int neighborIndices[27] = {0};

    #pragma omp parallel for schedule(static, 512) private(neighborIndices)
    for(size_t i = 0; i<particles.size(); i++)
    {
        auto& p = particles[i];
        if (p.isRigid) {continue;}

        p.density = 0.0f;
        glm::vec3 i_pos = p.pos;
        
        int ix, iy, iz;
        grid.posToIndex(ix, iy, iz, i_pos.x, i_pos.y, i_pos.z);

        int neighborIndices[27] = {0};
        int valid_index = grid.neighborCellIndices(neighborIndices, ix, iy, iz);

        for (int cellIdx=0; cellIdx<valid_index; cellIdx++) {
            const std::vector<int>& cell = grid.grid[neighborIndices[cellIdx]];
            glm::vec3 p_pos = p.pos;
            for (int j : cell) {
                glm::vec3 rij = p_pos - particles[j].pos;
                float r2 = glm::dot(rij, rij);
                if (r2 >= 0 && r2 <= h2) {
                    p.density += mass * Kernel::Density::W_poly_fast(r2);
                }
            }
        }
    }//);
}


void FluidSimulation::pressureFromDensity(){
    // calculate the pressure each particle has using its density
    
    // "Equation of State" with gamma fixed at 2
    #pragma omp parallel for simd schedule(static, 512)
    for(size_t i = 0; i<particles.size(); i++)
    {
        auto& p = particles[i];
        if (p.isRigid) {
            p.density = restDensity;
            p.pressure = 0.0f;
        } else {
            // float base = (p.density *  invRestDensity);
            // p.pressure = std::max(0.0f, (float) (kappa * (pow(base, 7) * (p.density * invRestDensity) - 1)));
            p.pressure = kappa * ((p.density *  invRestDensity) * (p.density * invRestDensity) - 1);
        }
    }
}

void FluidSimulation::forcesFromPressure(glm::quat rotation){
    // calculate the force of each particle using the pressure and density of surrounding particles

    //std::for_each(std::execution::unseq, particles.begin(), particles.end(), [this](Particle& p_i)

    int neighborIndices[27] = {0};

    #pragma omp parallel for schedule(static, 512) private(neighborIndices)
    for(size_t i = 0; i<particles.size(); i++)
    {
        auto& p_i = particles[i];
        if (p_i.isRigid) { continue; }
        glm::vec3 i_pos = p_i.pos;
        
        int ix, iy, iz;
        grid.posToIndex(ix, iy, iz, i_pos.x, i_pos.y, i_pos.z);
        int valid_index = grid.neighborCellIndices(neighborIndices, ix, iy, iz);

        auto force_i = glm::vec3{0.0};

        // Enforces boundary condition, if particles are close to the wall, generate a force repels them
        glm::vec3 wallNormal = glm::vec3(0.0f);
        float wallDist = 0.0f;
        if (ix == 0) {
            wallNormal = glm::vec3(1.0f, 0.0f, 0.0f);
            wallDist = abs(grid.xMin - p_i.pos.x);
            force_i += wallRepulsion * (h - wallDist) * (h - wallDist) * wallNormal;
        }
        if (ix == grid.gridSizeX-1) {
            wallNormal = glm::vec3(-1.0f, 0.0f, 0.0f);
            wallDist = abs(grid.xMax - p_i.pos.x);
            force_i += wallRepulsion * (h - wallDist) * (h - wallDist) * wallNormal;
        }
        if (iy == 0) {
            wallNormal = glm::vec3(0.0f, 1.0f, 0.0f);
            wallDist = abs(grid.yMin - p_i.pos.y);
            force_i += wallRepulsion * (h - wallDist) * (h - wallDist) * wallNormal;
        }
        if (iy == grid.gridSizeY-1) {
            wallNormal = glm::vec3(0.0f, -1.0f, 0.0f);
            wallDist = abs(grid.yMax - p_i.pos.y);
            force_i += wallRepulsion * (h - wallDist) * (h - wallDist) * wallNormal;
        }
        if (iz == 0) {
            wallNormal = glm::vec3(0.0f, 0.0f, 1.0f);
            wallDist = abs(grid.zMin - p_i.pos.z);
            force_i += wallRepulsion * (h - wallDist) * (h - wallDist) * wallNormal;
        }
        if (iz == grid.gridSizeZ-1) {
            wallNormal = glm::vec3(0.0f, 0.0f, -1.0f);
            wallDist = abs(grid.zMax - p_i.pos.z);
            force_i += wallRepulsion * (h - wallDist) * (h - wallDist) * wallNormal;
        }

        for (int cell_idx=0; cell_idx<valid_index; cell_idx++) {
            for (int j : grid.grid[neighborIndices[cell_idx]]) {
                Particle& p_j = particles[j];
                if (&p_i == &p_j) continue;

                glm::vec3 rij_vec = p_i.pos - p_j.pos;
                float r2 = glm::dot(rij_vec, rij_vec);

                if (r2 < h2 && r2 > 1e-9f) {                    
                    //glm::vec3 gradW = Kernel::Force::grad_wendland_3d(rij_vec, r2);
                    glm::vec3 gradW = Kernel::Force::gradW_poly_fast(rij_vec, r2);

                    // Symmetric Pressure Force formula
                    // Force = -mj * (pi/rhoi^2 + pj/rhoj^2) * gradW
                    float pressureTerm = (p_i.pressure / (p_i.density * p_i.density)) + 
                                         (p_j.pressure / (p_j.density * p_j.density));
                    
                    glm::vec3 forceTerm = -mass * pressureTerm * gradW;

                    force_i += forceTerm;
                    if (particles[j].isRigid) { particles[j].force -= forceTerm; } 
                }
            }
        }
        p_i.force = force_i + gravity;
    };//);
}

void FluidSimulation::velFromForces(glm::quat rotation){
    // Use the forces that we calculated to calculate the new position and velocity of each particle

    #pragma omp parallel for simd schedule(static, 512)
    for(size_t i = 0; i<particles.size(); i++)
    {
        auto& p = particles[i];
        if (p.isRigid) {continue;}

        // p.vel = p.vel * 0.999f + glm::rotate(glm::normalize(glm::conjugate(rotation)),p.force) * dt;
        // p.vel = p.vel + glm::rotate(glm::normalize(glm::conjugate(rotation)),p.force) * dt;
        p.vel = p.vel + p.force * dt;
        p.pos += p.vel * dt;

        // Ground
        if (p.pos.z < 0.01f) {
            p.pos.z = 0.01f;
            // p.vel.z *= -restitution;
        }

        // Roof for Rotation
        if (p.pos.z > 19.99f) {
            p.pos.z = 19.99f;
            // p.pos.z *= -restitution;
        }

        if (p.pos.x < -9.99f) {
            p.pos.x = -9.99f;
            // p.vel.x *= -restitution;
        }

        if (p.pos.x > 9.99f) {
            p.pos.x = 9.99f;
            // p.vel.x *= -restitution;
        }

         if (p.pos.y < -9.99f) {
            p.pos.y = -9.99f;
            // p.vel.y *= -restitution;
        }

        if (p.pos.y > 9.99f) {
            p.pos.y = 9.99f;
            // p.vel.y *= -restitution;
        }

    }
}

void FluidSimulation::rigidBodyCollisionAndBoundaries(glm::quat rotation) {
    for (size_t i = 0; i < cubes.size(); ++i) {
        constraint.ApplyTo(cubes[i]);
        for (size_t j = i+1; j < cubes.size(); ++j) {
            cubes[i].Collide(cubes[j]);
        }
        cubes[i].integrateFromBoundaryForces(rotation, dt);
        cubes[i].updateBoundaryParticles();
    }
}