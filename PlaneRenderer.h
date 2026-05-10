#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "CubeMesh.h"

class PlaneRenderer
{
private: 
    CubeMesh cubeMesh;

public:

    glm::vec3 position;
    glm::vec3 normal;

    float size;
    float thickness;

    bool visible;

    PlaneRenderer();

    void render(Shader& shader);
};
