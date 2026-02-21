#include "PositionLoader.h"
#include <dirent.h>
#include <algorithm>
#include <cstdio>
#include <iostream>

FrameData PositionLoader::load(const std::string& filename)
{
    FrameData frame;

    FILE* fp = fopen(filename.c_str(),"rb");
    if(!fp)
    {
        std::cerr<<"Cannot open "<<filename<<std::endl;
        return frame;
    }

    int N=0;
    if(fread(&N,sizeof(int),1,fp)!=1)
    {
        fclose(fp);
        return frame;
    }

    std::vector<float> posf(N*3);
    std::vector<float> rf(N);

    fread(&posf[0],sizeof(float),N*3,fp);
    fread(&rf[0],sizeof(float),N,fp);

    fclose(fp);

    frame.pos.resize(N);
    frame.r.resize(N);

    for(int i=0;i<N;i++)
    {
        frame.pos[i]=glm::vec3(
            posf[i*3+0],
            posf[i*3+1],
            posf[i*3+2]
        );
        frame.r[i]=rf[i];
    }

    return frame;
}

std::vector<FrameData> PositionLoader::loadAllFrames(const std::string& folder)
{
    std::vector<std::string> filenames;
    std::vector<FrameData> frames;

    DIR* dir = opendir(folder.c_str());
    if(!dir)
    {
        std::cerr<<"Cannot open folder "<<folder<<std::endl;
        return frames;
    }

    struct dirent* ent;

    while((ent=readdir(dir))!=NULL)
    {
        std::string name = ent->d_name;

        if(name.size()>4 && name.substr(name.size()-4)==".bin")
        {
            filenames.push_back(folder+"/"+name);
        }
    }

    closedir(dir);

    std::sort(filenames.begin(),filenames.end());

    for(size_t i=0;i<filenames.size();i++)
    {
        frames.push_back(load(filenames[i]));
    }

    return frames;
}
