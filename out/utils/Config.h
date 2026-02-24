#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>
#include <cmath>
#include "Figure.h"

class Config{
public:
    float posCam[3], lookAt[3], up[3], projection[3];
    float fov, near, far;
    int width, height;
    std::vector<Figure> models;
    Config();
    bool loadXML(const char* xmlPath);
};
#endif