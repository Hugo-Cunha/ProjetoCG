#ifndef GROUP_H
#define GROUP_H

#include <vector>
#include <string>
#include "Figure.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Classe Base Abstrata para Transformações
class Transformation {
public:
    virtual ~Transformation() = default;
    virtual void apply() = 0;
};

class Translate : public Transformation {
    float x, y, z;
public:
    Translate(float x, float y, float z) : x(x), y(y), z(z) {}
    void apply() override { glTranslatef(x, y, z); }
};

class Rotate : public Transformation {
    float angle, x, y, z;
public:
    Rotate(float a, float x, float y, float z) : angle(a), x(x), y(y), z(z) {}
    void apply() override { glRotatef(angle, x, y, z); }
};

class Scale : public Transformation {
    float x, y, z;
public:
    Scale(float x, float y, float z) : x(x), y(y), z(z) {}
    void apply() override { glScalef(x, y, z); }
};

// Classe Group: O nó da nossa árvore (Scene Graph)
class Group {
public:
    std::vector<Transformation*> transforms;
    std::vector<Figure> models;
    std::vector<Group*> children;
    Group() = default;
    ~Group();
    void draw();
};

#endif