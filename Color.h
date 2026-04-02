#pragma once
#include <cstdio>

class Color {
    public:
    float r, g, b,h,s,v;
    void hsv2rgb(float h, float s, float v);
    void make_hue(int i);
};

