#ifndef FIGURE_H
#define FIGURE_H

#include <GL/glew.h>

#include <vector>
#include <string>
#include <cmath>
#include "Point.h"


// Classe base para todas as figuras
class Figure
{
protected:
    std::vector<Point> vertices;
    unsigned int vbo;   // ID do buffer na placa gráfica
    int vertexCount;    // Quantidade de vértices a desenhar

public:
    Figure() : vbo(0), vertexCount(0) {}
    virtual ~Figure() = default;
    void addTriangle(Point p1, Point p2, Point p3);
    void save(const std::string &filename);
    bool load(const std::string &filename);
    void draw();
    void prepareVBO();
};

// Primitivas
class Plane : public Figure
{
public:
    Plane(float size, int divisions);
};

class Box : public Figure
{
public:
    Box(float size, int divisions);
};

class Sphere : public Figure
{
public:
    Sphere(float radius, int slices, int stacks);
};

class Cone : public Figure
{
public:
    Cone(float radius, float height, int slices, int stacks);
};

class Ring : public Figure
{
public:
    Ring(float ri, float re, int slices);
};

class Asteroid : public Figure
{
public:
    Asteroid(float radius, int slices, int stacks, float roughness);
};

class BezierPatch : public Figure {
public:
    // Construtor que gera o bule ou qualquer superfície de Bézier
    BezierPatch(const std::string &patchFile, int tessellation);

private:
    // Funções auxiliares para o cálculo matemático
    float bernstein(int i, float t);
    Point getBezierPoint(float u, float v, const std::vector<Point>& controlPoints, const std::vector<int>& indices);
};
#endif

