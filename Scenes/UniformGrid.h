
#pragma once

#include "ParticlePool.h"

class UniformGrid 
{
private:
    struct GridCell 
    {
        std::vector<int> particleIndices;
    };

    glm::vec3 m_lowCorner;
    glm::vec3 m_highCorner;
    glm::ivec3 m_partition;
    glm::vec3 m_cellScale;

    std::vector<GridCell> m_cells;

    glm::ivec3 computeCellCoord(const glm::vec3& position) const noexcept;
    int computeCellIndex(const glm::ivec3& cellCoord) const noexcept;

public:
    UniformGrid() = default;
    UniformGrid(const glm::vec3& centre, const glm::vec3& scale, const glm::ivec3& partition) noexcept;

    void updateCells(const ParticlePool& pool) noexcept;

    template <class Function>
    void forEachNeighbor(const ParticlePool& pool, size_t particleIndex, Function&& func) const noexcept 
    {
        const glm::ivec3 cellCoord = computeCellCoord(pool.positions[particleIndex]);
        const glm::ivec3 startOffset = {
                (cellCoord.x == 0) ? 0 : -1,
                (cellCoord.y == 0) ? 0 : -1,
                (cellCoord.z == 0) ? 0 : -1,
        };
        const glm::ivec3 endOffset = {
            (cellCoord.x == m_partition.x - 1) ? 0 : 1,
            (cellCoord.y == m_partition.y - 1) ? 0 : 1,
            (cellCoord.z == m_partition.z - 1) ? 0 : 1,
        };

        for (int dz = startOffset.z; dz <= endOffset.z; ++dz) 
        {
            for (int dy = startOffset.y; dy <= endOffset.y; ++dy) 
            {
                for (int dx = startOffset.x; dx <= endOffset.x; ++dx) 
                {
                    const glm::ivec3 neighborCellCoord = cellCoord + glm::ivec3(dx, dy, dz);
                    const int neighborCellIndex = computeCellIndex(neighborCellCoord);
                    const std::vector<int>& particleIndices = m_cells[neighborCellIndex].particleIndices;

                    for (int index : particleIndices) 
                    {
                        func(index);
                    }
                }
            }
        }
    }
};

