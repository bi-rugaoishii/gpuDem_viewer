#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct FrameData
{
    std::vector<glm::vec3> pos;
    std::vector<float> r;
};

class PositionLoader {
public:
    FrameData load(const std::string& filename);
    std::vector<FrameData> loadAllFrames(const std::string& folder);
};
