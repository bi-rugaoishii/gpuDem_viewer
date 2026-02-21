// Sphere.h
#pragma once
#include <vector>
#include <glad/glad.h>

class Sphere {
public:
    Sphere(float radius, int sectorCount, int stackCount);
    void draw();
    ~Sphere();
private:
    GLuint VAO, VBO, EBO;
    unsigned int indexCount;
};