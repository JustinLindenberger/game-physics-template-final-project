
#include <imgui.h>
#include "Scene.h"
#include "Solver.h"
#include "Container.h"
#include "Wall.h"

typedef Poly6Kernel Kernel;

class Scene1 final : public Scene
{
private:
    static constexpr int32_t SIZE = 24;
    static constexpr float SPACING = 0.2f;

    float m_stepTime = 0.02f;
    float m_lag = 0.0f;
    bool m_paused = true;

    ParticlePool m_pool;
    Container m_container;
    Wall m_wall;
    UniformGrid m_grid;
    Solver<Kernel> m_solver;

public:
    void init() override;
    void simulateStep() override;
    void onDraw(Renderer& renderer) override;
    void onGUI() override;
};

