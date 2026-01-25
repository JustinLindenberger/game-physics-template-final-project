
#include "Scene1.h"

void Scene1::init()
{
    constexpr int32_t numParticles = SIZE * SIZE * SIZE;
    constexpr float simulationScale = 0.4f;

    constexpr glm::vec3 containerScale = glm::vec3(simulationScale * SIZE);
    constexpr glm::vec3 containerCentre = glm::vec3(0.0f, containerScale.y / 6.0f, 0.0f);
    constexpr float damping = 0.8f;

    constexpr glm::ivec3 gridPartition = {
        static_cast<int>(containerScale.x / Kernel::RADIUS),
        static_cast<int>(containerScale.y / Kernel::RADIUS),
        static_cast<int>(containerScale.z / Kernel::RADIUS),
    };

    constexpr glm::vec3 wallCentre = containerCentre + glm::vec3(0.0f, containerScale.y / 4.0f, containerScale.z / 12.0f);
    constexpr glm::vec3 wallScale = glm::vec3(containerScale.x, containerScale.y / 6.0f, 5.0f * containerScale.z / 6.0f);

    m_container = Container(containerCentre, containerScale, damping);
    m_wall = Wall(wallCentre, wallScale, damping);
    m_grid = UniformGrid(containerCentre, containerScale, gridPartition);
    m_solver = Solver<Kernel>(numParticles);

    int32_t i = 0;
    m_pool.resize(numParticles);

    for (int32_t z = -SIZE/2; z < (SIZE+1)/2; ++z)
    {
        for (int32_t y = -SIZE/2; y < (SIZE+1)/2; ++y)
        {
            for (int32_t x = -SIZE/2; x < (SIZE+1)/2; ++x)
            {
                m_pool.positions[i++] = SPACING * glm::vec3(x, y, z);
            }
        }
    }
}

void Scene1::simulateStep()
{
    if (m_paused) return;

    m_lag += ImGui::GetIO().DeltaTime;
    while (m_lag > m_stepTime) 
    {
        m_grid.updateCells(m_pool);
        m_solver.step(m_pool, m_grid, m_stepTime);
        m_container.bound(m_pool);
        m_wall.bound(m_pool);
        m_lag -= m_stepTime;
    }
}

void Scene1::onDraw(Renderer& renderer)
{
    m_container.draw(renderer);
    m_wall.draw(renderer);
    m_pool.draw(renderer); 
    // m_pool.drawWithSpeed(renderer);
}

void Scene1::onGUI()
{
    ImGui::SliderFloat("Step Time", &m_stepTime, 0.001f, 0.1f);
    if (ImGui::Button(m_paused ? "PLAY" : "PAUSE"))
        m_paused = !m_paused;
}

