#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

class CubeMesh
{
public:

    GLuint VAO;
    GLuint VBO;

    CubeMesh();

    void draw();
};
