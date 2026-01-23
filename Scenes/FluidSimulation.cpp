#include "FluidSimulation.h"
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
}


void FluidSimulation::simulateStep(){
    reset();
    insertParticelsIntoGrid();
    densityCalculations();
    pressureFromDensity();
    forcesFromPressure();
    velFromForces();
}



void FluidSimulation::reset(){
    // reset e.g. densities or the grid
}

void FluidSimulation::insertParticelsIntoGrid(){
    // insert particles at their current locaiton into the grid
}

void FluidSimulation::densityCalculations(){
    // calculate the density based on the position of the particles
}

void FluidSimulation::pressureFromDensity(){
    // calculate the pressure each particle has using its density
}

void FluidSimulation::forcesFromPressure(){
    // calculate the force of each particle using the pressure and density of surrounding particles
}

void FluidSimulation::velFromForces(){
    // Use the forces that we calculated to calculate the new position and velocity of each particle

    // DEBUG
    std::for_each(std::execution::par_unseq, particles.begin(), particles.end(), [this](Particle& p){
        p.pos += gravity * 0.001f; // doenst make sense, just to make the particles move
    }); 
    // ABOVE IS EQUIVALENT TO:
    // for(auto& p: particles){
    //     p.pos += gravity * 0.001f; // doenst make sense, just to make the particles move
    // }
}