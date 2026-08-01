#include "PositionLoader.h"

#include <dirent.h>

#include <algorithm>
#include <cstdio>
#include <iostream>

FrameData PositionLoader::load(const std::string& filename){
    FrameData frame;

    FILE* fp = fopen(filename.c_str(), "rb");
    if(fp == NULL){
        std::cerr << "Cannot open " << filename << std::endl;
        return frame;
    }

    int particleCount = 0;
    if(fread(&particleCount, sizeof(int), 1, fp) != 1){
        std::cerr << "Cannot read particle count from " << filename << std::endl;
        fclose(fp);
        return frame;
    }

    if(particleCount < 0){
        std::cerr << "Invalid particle count in " << filename << std::endl;
        fclose(fp);
        return frame;
    }

    const std::size_t count = static_cast<std::size_t>(particleCount);
    std::vector<float> positionBuffer(count * 3);

    frame.pos.resize(count);
    frame.r.resize(count);

    if(count > 0){
        const std::size_t positionValueCount = count * 3;

        if(fread(positionBuffer.data(), sizeof(float), positionValueCount, fp) != positionValueCount){
            std::cerr << "Cannot read positions from " << filename << std::endl;
            fclose(fp);
            return FrameData();
        }

        if(fread(frame.r.data(), sizeof(float), count, fp) != count){
            std::cerr << "Cannot read radii from " << filename << std::endl;
            fclose(fp);
            return FrameData();
        }
    }

    fclose(fp);

    for(std::size_t i = 0; i < count; i++){
        frame.pos[i] = glm::vec3(
            positionBuffer[i * 3 + 0],
            positionBuffer[i * 3 + 1],
            positionBuffer[i * 3 + 2]
        );
    }

    frame.isValid = true;
    return frame;
}

std::vector<std::string> PositionLoader::listFrameFiles(const std::string& folder){
    std::vector<std::string> filenames;

    DIR* dir = opendir(folder.c_str());
    if(dir == NULL){
        std::cerr << "Cannot open folder " << folder << std::endl;
        return filenames;
    }

    struct dirent* entry = NULL;

    while((entry = readdir(dir)) != NULL){
        const std::string name = entry->d_name;

        if(name.size() > 4 && name.substr(name.size() - 4) == ".bin"){
            filenames.push_back(folder + "/" + name);
        }
    }

    closedir(dir);

    std::sort(filenames.begin(), filenames.end());
    return filenames;
}
