#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

struct StlTriangle {
    glm::vec3 normal;
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;
};

class StlMeshLoader {
public:
    bool loadASCII(const std::string& filename);
    const std::vector<StlTriangle>& getTriangles() const;

private:
    std::vector<StlTriangle> triangles;
};