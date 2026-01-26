#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Constants.h"



class Grid{
    private:
        const float xMin =  -10.0f;
        const float xMax =  10.0f;
        const float yMin =  -10.0f;
        const float yMax =  10.0f;
        const float zMin =  0.0f;
        const float zMax = 20.0f;

        const int gridSizeX = int((xMax-xMin) / h) + 1;
        const int gridSizeY = int((yMax-yMin) / h) + 1; 
        const int gridSizeZ = int((zMax-zMin) / h) + 1;

        const float cellSizeX = (xMax-xMin) / gridSizeX;
        const float cellSizeY = (yMax-yMin) / gridSizeY;
        const float cellSizeZ = (zMax-zMin) / gridSizeZ;

    public:
        std::vector<std::vector<int>> grid;

        // construct the grid
        Grid() {
                grid.resize(gridSizeX * gridSizeY * gridSizeZ);
            }

        // given an index in each direction, return the index of the grid vector, where this gridcell is stored
        inline int index(int ix, int iy, int iz) const {
            return ix + iy * gridSizeX + iz * gridSizeX * gridSizeY;
        }


        //given a position(x,y,z) return the index of the gridcell
        // e.g. given a grid (-5,5) in each axis, and 10 grid cells,
        // (3.2, -9.9, 1.2) would return (8,0,6)
        inline void posToIndex(int& ix, int& iy, int& iz, float x, float y, float z) const {
            ix = int((x-xMin) / cellSizeX);
            iy = int((y-yMin) / cellSizeY);
            iz = int((z-zMin) / cellSizeZ);

            // TODO(optimization): remove once we ensure that no particles are outside the bounds for better performance
            ix = glm::clamp(ix, 0, gridSizeX - 1);
            iy = glm::clamp(iy, 0, gridSizeY - 1);
            iz = glm::clamp(iz, 0, gridSizeZ - 1);
        }

        // get the indices of the neighboring cells in each axis (27 in total)
        inline int neighborCellIndices(int* neighborCells, int ix, int iy, int iz) const {
            int valid_until = 0;
            for (int dx = -1; dx <= 1; ++dx)
            for (int dy = -1; dy <= 1; ++dy)
            for (int dz = -1; dz <= 1; ++dz) {

                int nx = ix + dx;
                int ny = iy + dy;
                int nz = iz + dz;

                // skip out-of-bounds cells
                if (nx < 0 || nx >= gridSizeX ||
                    ny < 0 || ny >= gridSizeY ||
                    nz < 0 || nz >= gridSizeZ)
                    continue;

                int cellIndex =
                    nx +
                    ny * gridSizeX +
                    nz * gridSizeX * gridSizeY;

                neighborCells[valid_until++] = cellIndex;
            }

            return valid_until;
        }

        // empties the grid
        void clear() {
            for (std::vector<int>& gridcell : grid)
                gridcell.clear();
        }

        // inserts partices at the correct place in the map 
        inline void insertParticle(const glm::vec3& pos, int particleIndex) {
            int ix, iy, iz;
            posToIndex(ix, iy, iz, pos.x, pos.y, pos.z);
            
            int grid_index = index(ix, iy, iz);
            grid[grid_index].push_back(particleIndex);
        }

};