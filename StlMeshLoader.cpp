#include "StlMeshLoader.h"
#include <fstream>
#include <sstream>

bool StlMeshLoader::loadASCII(const std::string& filename)
{
    triangles.clear();

    std::ifstream file(filename);
    if(!file) return false;

    std::string line;
    StlTriangle tri;

    while(std::getline(file,line))
    {
        std::stringstream ss(line);
        std::string word;
        ss >> word;

        if(word == "facet")
        {
            std::string normalStr;
            ss >> normalStr;
            ss >> tri.normal.x >> tri.normal.y >> tri.normal.z;
        }
        else if(word == "vertex")
        {
            glm::vec3 v;
            ss >> v.x >> v.y >> v.z;

            if(tri.v0 == glm::vec3(0))
                tri.v0 = v;
            else if(tri.v1 == glm::vec3(0))
                tri.v1 = v;
            else
            {
                tri.v2 = v;
                triangles.push_back(tri);
                tri = StlTriangle();
            }
        }
    }

    return true;
}

const std::vector<StlTriangle>&
StlMeshLoader::getTriangles() const
{
    return triangles;
}