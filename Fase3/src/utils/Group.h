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
    float time;
    bool align;
    std::vector<Point> p;

public:
    Translate(float t, bool a, std::vector<Point> points);
    void apply() override;

private:
    // Funções auxiliares para a curva [cite: 122, 134]
    void getGlobalCatmullRomPoint(float gt, float *pos, float *deriv);
    void getCatmullRomPoint(float t, Point p0, Point p1, Point p2, Point p3, float *pos, float *deriv);
    void applyAlignment(float *deriv);
};

class Rotate : public Transformation {
    float angle, time, x, y, z;
    bool isAnimated;

public:
    
    Rotate(float val, float x, float y, float z, bool animated) 
        : x(x), y(y), z(z), isAnimated(animated) { // val pode ser ângulo (estático) ou tempo (animado) 
        if (animated) { this->time = val; this->angle = 0; }
        else { this->angle = val; this->time = 0; }
    }

    void apply() override;
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