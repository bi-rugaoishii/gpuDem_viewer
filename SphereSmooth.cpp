#include "SphereSmooth.h"

SphereSmooth::SphereSmooth(float radius, int sectorCount, int stackCount)
    : VAO(0), VBO(0), EBO(0), indexCount(0)
{
    build(radius, sectorCount, stackCount);
}

SphereSmooth::~SphereSmooth()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void SphereSmooth::build(float radius, int sectorCount, int stackCount)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    float x, y, z, xy;
    float nx, ny, nz;

    float sectorStep = 2.0f * static_cast<float>(M_PI) / sectorCount;
    float stackStep  = static_cast<float>(M_PI) / stackCount;

    float sectorAngle, stackAngle;

    // ===== 頂点生成 =====
    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = static_cast<float>(M_PI) / 2.0f - i * stackStep;
        xy = radius * cosf(stackAngle);
        z  = radius * sinf(stackAngle);

        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);

            // position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // normal
            nx = x / radius;
            ny = y / radius;
            nz = z / radius;

            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        }
    }

    // ===== インデックス生成 =====
    int k1, k2;
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if(i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    indexCount = static_cast<int>(indices.size());

    // ===== OpenGL =====
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(float),
                 &vertices[0],
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int),
                 &indices[0],
                 GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void SphereSmooth::bind() const
{
    glBindVertexArray(VAO);
}

int SphereSmooth::getIndexCount() const
{
    return indexCount;
}
