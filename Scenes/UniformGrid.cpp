
#include "UniformGrid.h"

UniformGrid::UniformGrid(const glm::vec3& centre, const glm::vec3& scale, const glm::ivec3& partition) noexcept :
    m_lowCorner(centre - 0.5f * scale), m_highCorner(centre + 0.5f * scale), m_partition(partition) 
{
    m_cellScale = glm::vec3{
        scale.x / partition.x,
        scale.y / partition.y,
        scale.z / partition.z,
    };

    const size_t numCells = partition.x * partition.y * partition.z;
    m_cells.resize(numCells);
    
    for (GridCell& cell : m_cells) 
    {
        cell.particleIndices.reserve(128);
    }
}

glm::ivec3 UniformGrid::computeCellCoord(const glm::vec3& position) const noexcept 
{
    const glm::vec3 relativePosition = position - m_lowCorner;
    const int x = static_cast<int>(relativePosition.x / m_cellScale.x);
    const int y = static_cast<int>(relativePosition.y / m_cellScale.y);
    const int z = static_cast<int>(relativePosition.z / m_cellScale.z);
    return glm::ivec3(x, y, z);
}

int UniformGrid::computeCellIndex(const glm::ivec3& cellCoord) const noexcept 
{
    return cellCoord.x + m_partition.x * (cellCoord.y + m_partition.y * cellCoord.z);
}

void UniformGrid::updateCells(const ParticlePool& pool) noexcept 
{
    # pragma omp parallel for schedule(static)
    for (GridCell& cell : m_cells) 
    {
        cell.particleIndices.clear();
    }
    
    # pragma omp parallel for schedule(static)
    for (size_t i = 0; i < pool.size; ++i) 
    {
        const glm::ivec3 cellCoord = computeCellCoord(pool.positions[i]);
        const int cellIndex = computeCellIndex(cellCoord);
        m_cells[cellIndex].particleIndices.emplace_back(i);
    }
}
