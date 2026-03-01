#pragma once
#include <vector>
#include <glm/glm.hpp>

class Mesh {
public:
    void build(const std::vector<glm::vec3>& vertices,
               const std::vector<glm::vec3>& normals);

    void draw() const;

private:
    unsigned int VAO=0,VBO=0;
    int vertexCount=0;
};