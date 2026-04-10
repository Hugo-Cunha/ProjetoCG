#include "Config.h"
#include <iostream>

using namespace tinyxml2;

Config::Config() : width(800), height(800), fov(45.0f), near(1.0f), far(1000.0f), root(nullptr) {
    for(int i=0; i<3; i++) posCam[i] = lookAt[i] = up[i] = 0.0f;
}

Config::~Config() {
    delete root;
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

    XMLElement* lightsElement = root->FirstChildElement("lights");
    if (lightsElement) {
        for (XMLElement* lightXML = lightsElement->FirstChildElement("light"); lightXML; lightXML = lightXML->NextSiblingElement("light")) {
            Light l;
            const char* typeStr = lightXML->Attribute("type");
            if (typeStr) l.type = typeStr;

            // ACEITA TANTO "spotlight" COMO "spot"
            if (l.type == "point" || l.type == "spotlight" || l.type == "spot") {
                l.pos[0] = lightXML->FloatAttribute("posx");
                l.pos[1] = lightXML->FloatAttribute("posy");
                l.pos[2] = lightXML->FloatAttribute("posz");
                l.pos[3] = 1.0f; 
            }
            if (l.type == "directional" || l.type == "spotlight" || l.type == "spot") {
                l.dir[0] = lightXML->FloatAttribute("dirx"); // ATENÇÃO AO X MAIÚSCULO
                l.dir[1] = lightXML->FloatAttribute("diry");
                l.dir[2] = lightXML->FloatAttribute("dirz");
                l.dir[3] = 0.0f; 
            }
            if (l.type == "spotlight" || l.type == "spot") {
                // Se não puseres cutoff no XML, ele assume 45 graus (um cone realista)
                l.cutoff = lightXML->FloatAttribute("cutoff", 45.0f); 
            }
            
            lights.push_back(l);
        }
    }

    // 3. Models
    XMLElement* g = root->FirstChildElement("group");
    if (!g){
        std::cout << "[Config] No <group> in <world>" << std::endl;
        return true; // cena vazia
    }

    this->root = new Group();
    std::cout << "[Config] Parsing root <group>" << std::endl;
    parseGroup(g, this->root);

    return true;
}


void Config::parseGroup(XMLElement* groupElement, Group* currentGroup) {
    std::cout << "[Config] New group at " << groupElement << std::endl;
    for (XMLElement* child = groupElement->FirstChildElement(); child; child = child->NextSiblingElement()) {
        std::string name = child->Value();
        std::cout << "  [Config] child node: " << name << std::endl;

        // 1. Transformações
        if (name == "transform") {
            for (XMLElement* t = child->FirstChildElement(); t; t = t->NextSiblingElement()) {
                std::string tName = t->Value();
                if (tName == "translate") {
                    if (t->Attribute("time")) { // Caso ANIMADO (Fase 3
                        float time = t->FloatAttribute("time");
                        bool align = t->BoolAttribute("align");
                        std::vector<Point> points;

                        // Percorrer os sub-elementos <point>
                        for (tinyxml2::XMLElement* p = t->FirstChildElement("point"); p; p = p->NextSiblingElement("point")) {
                            points.push_back(Point(p->FloatAttribute("x"), p->FloatAttribute("y"), p->FloatAttribute("z")));
                        }
                        currentGroup->transforms.push_back(
                            new Translate(time, align, points)
                        );
                    } 
                    else { // Caso ESTÁTICO (Fase 1/2)
                        std::vector<Point> p;
                        p.push_back(Point(t->FloatAttribute("x"), t->FloatAttribute("y"), t->FloatAttribute("z")));
                        currentGroup->transforms.push_back(
                            new Translate(0, false, p)
                        );
                    }
                }
                else if (tName == "rotate") {
                    float x = t->FloatAttribute("x");
                    float y = t->FloatAttribute("y");
                    float z = t->FloatAttribute("z");

                    if (t->Attribute("time")) { // Caso ANIMADO (Fase 3) 
                        float time = t->FloatAttribute("time");
                        currentGroup->transforms.push_back(
                            new Rotate(time, x, y, z, true)  // true indica que usa tempo
                        );
                    } 
                    else { // Caso ESTÁTICO (Fase 1/2)
                        float angle = t->FloatAttribute("angle");
                        currentGroup->transforms.push_back(
                            new Rotate(angle, x, y, z, false)
                        );
                    }
                }
                else if (tName == "scale"){
                    currentGroup->transforms.push_back(
                        new Scale(t->FloatAttribute("x"), t->FloatAttribute("y"), t->FloatAttribute("z")));
                    }
            }
        }
        // 2. Modelos
        else if (name == "models") {
            for (XMLElement* m = child->FirstChildElement("model"); m; m = m->NextSiblingElement("model")) {
                const char* fname = m->Attribute("file");
                std::cout << "[Config] Loading model: " << (fname ? fname : "(null)") << std::endl;
                Figure f;
                if (fname && f.load(fname)) {
                    
                    // --- CÓDIGO NOVO (TEXTURAS E MATERIAIS) ENTRA AQUI ---
                    XMLElement* tex = m->FirstChildElement("texture");
                    if (tex && tex->Attribute("file")) {
                        f.textureFile = tex->Attribute("file");
                    }
                    XMLElement* color = m->FirstChildElement("color");
                    if (color) {
                        // Função lambda para ler os componentes R, G, B e converter para [0, 1]
                        auto readColor = [](XMLElement* elem, float* arr) {
                            if (elem) {
                                arr[0] = elem->FloatAttribute("R") / 255.0f;
                                arr[1] = elem->FloatAttribute("G") / 255.0f;
                                arr[2] = elem->FloatAttribute("B") / 255.0f;
                                arr[3] = 1.0f; // Alpha (Opacidade)
                            }
                        };

                        readColor(color->FirstChildElement("diffuse"), f.diffuse);
                        readColor(color->FirstChildElement("ambient"), f.ambient);
                        readColor(color->FirstChildElement("specular"), f.specular);
                        readColor(color->FirstChildElement("emissive"), f.emissive);

                        XMLElement* shininess = color->FirstChildElement("shininess");
                        if (shininess) f.shininess = shininess->FloatAttribute("value");
                    }
                    // -----------------------------------------------------

                    f.loadTextureToGPU();
                    f.prepareVBO();
                    currentGroup->models.push_back(f);
                    std::cout << "[Config]   -> ok" << std::endl;
                } else {
                    std::cout << "[Config]   -> FAILED to load" << std::endl;
                }
            }
        }
        // 3. Subgrupos (Recursividade!) 
        else if (name == "group") {
            Group* newChild = new Group();
            currentGroup->children.push_back(newChild);
            parseGroup(child, newChild);
        }

    }
}