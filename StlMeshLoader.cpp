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
    int vCount=0;

    while(std::getline(file,line)){
        std::stringstream ss(line);
        std::string word;
        ss >> word;

        if(word == "facet"){
            std::string normalStr;
            ss >> normalStr;
            ss >> tri.normal.x >> tri.normal.y >> tri.normal.z;
        }else if(word == "vertex"){
            glm::vec3 v;
            ss >> v.x >> v.y >> v.z;

            if(vCount == 0){
                tri.v0 = v;
                vCount += 1;
            }else if(vCount == 1){
                tri.v1 = v;
                vCount += 1;
            }else{
                tri.v2 = v;
                triangles.push_back(tri);
                tri = StlTriangle();
                vCount = 0;
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
