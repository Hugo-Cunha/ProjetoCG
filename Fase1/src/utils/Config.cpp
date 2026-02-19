#include "Config.h"
#include "../tinyXML/tinyxml2.h"
#include <iostream>

using namespace tinyxml2;

Config::Config() : width(800), height(800), fov(45.0f), near(1.0f), far(1000.0f) {
    // Inicializar arrays a zero por segurança
    for(int i=0; i<3; i++) posCam[i] = lookAt[i] = up[i] = 0.0f;
}

bool Config::loadXML(const char* xmlPath) {
    XMLDocument doc;
    if (doc.LoadFile(xmlPath) != XML_SUCCESS) return false;

    XMLElement* root = doc.FirstChildElement("world");
    if (!root) return false;

    // 1. Window
    XMLElement* win = root->FirstChildElement("window");
    if (win) {
        width = win->IntAttribute("width");
        height = win->IntAttribute("height");
    }

    // 2. Camera
    XMLElement* cam = root->FirstChildElement("camera");
    if (cam) {
        XMLElement* pos = cam->FirstChildElement("position");
        if (pos) {
            posCam[0] = pos->FloatAttribute("x");
            posCam[1] = pos->FloatAttribute("y");
            posCam[2] = pos->FloatAttribute("z");
        }

        XMLElement* la = cam->FirstChildElement("lookAt");
        if (la) {
            lookAt[0] = la->FloatAttribute("x");
            lookAt[1] = la->FloatAttribute("y");
            lookAt[2] = la->FloatAttribute("z");
        }

        XMLElement* u = cam->FirstChildElement("up");
        if (u) {
            up[0] = u->FloatAttribute("x");
            up[1] = u->FloatAttribute("y");
            up[2] = u->FloatAttribute("z");
        }

        XMLElement* proj = cam->FirstChildElement("projection");
        if (proj) {
            fov = proj->FloatAttribute("fov");
            near = proj->FloatAttribute("near");
            far = proj->FloatAttribute("far");
        }
    }

    // 3. Models (dentro do group)
    XMLElement* group = root->FirstChildElement("group");
    if (group) {
        XMLElement* mods = group->FirstChildElement("models");
        if (mods) {
            for (XMLElement* m = mods->FirstChildElement("model"); m; m = m->NextSiblingElement("model")) {
                Figure f;
                const char* filename = m->Attribute("file");
                if (filename && f.load(filename)) {
                    models.push_back(f);
                }
            }
        }
    }
    return true;
}