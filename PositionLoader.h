#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct FrameData{
    std::vector<glm::vec3> pos;
    std::vector<float> r;
    bool isValid = false;
};

class PositionLoader{
public:
    FrameData load(const std::string& filename);

    // データ本体ではなく、ファイル名一覧だけ返す
    std::vector<std::string> listFrameFiles(const std::string& folder);
};
