#ifndef SPHERE_SMOOTH_H
#define SPHERE_SMOOTH_H

#include <vector>
#include <cmath>
#include <glad/glad.h>

class SphereSmooth {
public:
    SphereSmooth(float radius, int sectorCount, int stackCount);
    ~SphereSmooth();

    void bind() const;
    int getIndexCount() const;

private:
    void build(float radius, int sectorCount, int stackCount);

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    int indexCount;
};

#endif
