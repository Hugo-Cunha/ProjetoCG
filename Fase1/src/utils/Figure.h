#ifndef FIGURE_H
#define FIGURE_H

#include <vector>
#include <string>
#include <cmath>
#include "Point.h"

// Classe base para todas as figuras
class Figure {
protected:
    std::vector<Point> vertices;
public:
    virtual ~Figure() = default;
    void addTriangle(Point p1, Point p2, Point p3);
    void save(const std::string& filename);
    bool load(const std::string& filename);
    void draw();
};

// Primitivas
class Plane : public Figure {
public:
    Plane(float size, int divisions);
};

class Box : public Figure {
public:
    Box(float size, int divisions);
};

class Sphere : public Figure {
public:
    Sphere(float radius, int slices, int stacks);
};

class Cone : public Figure {
public:
    Cone(float radius, float height, int slices, int stacks);
};
#endif