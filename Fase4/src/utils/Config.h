#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>
#include <cmath>
#include "Figure.h"
#include "Group.h"
#include "../tinyXML/tinyxml2.h"

// Estrutura para guardar os dados de cada luz
struct Light {
    std::string type;
    float pos[4] = {0, 0, 0, 1}; // 1 no fim = Posição (Point/Spot)
    float dir[4] = {0, 0, 0, 0}; // 0 no fim = Direção (Directional)
    float cutoff = 180.0f;       // Por defeito não tem cutoff (180)
};

class Config{
public:
    float posCam[3], lookAt[3], up[3], projection[3];
    float fov, near, far;
    int width, height;
    std::vector<Figure> models;
    std::vector<Light> lights;

    Group* root;

    Config();
    ~Config();
    bool loadXML(const char* xmlPath);
private:
    void parseGroup(tinyxml2::XMLElement* groupElement, Group* currentGroup);
};
#endif