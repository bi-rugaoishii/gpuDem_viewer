#include "Color.h"
#include <cmath>

#define MAX_NUM_DATA 10.f
void Color::hsv2rgb(float h, float s, float v){
    float c = v * s;
    float x = c * (1.0f - std::fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    if (h < 60)       {r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else              { r = c; g = 0; b = x; }

    r +=m;
    g +=m;
    b +=m;
}

void Color::make_hue(int i){
    if(i==0){
        r=1.0f;
        g=1.0f;
        b=1.0f;
    }else{
        float hue = 360.0f*i/MAX_NUM_DATA;
        hsv2rgb(hue, 0.5f, 0.9f);
    }
}
