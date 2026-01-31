#pragma once
#include <glm/glm.hpp>
#include <vector>

// Smoothing radius
constexpr float h{0.3f};         

// h^2 and higher
constexpr float h2{h*h};
constexpr float h3{h*h*h};
constexpr float h4{h2*h2};
constexpr float h5{h2*h2*h};
constexpr float h6{h2*h2*h2};
constexpr float h9{h6*h2*h};

// pi
constexpr float pi = 3.1415f;

//gravity
extern glm::vec3 gravity;