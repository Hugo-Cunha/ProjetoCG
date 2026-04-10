#ifndef POINT_H
#define POINT_H

struct Point {
    // 1. Posição (x, y, z)
    float x, y, z;
    // 2. Normais (nx, ny, nz)
    float nx, ny, nz;
    // 3. Texturas (u, v)
    float u, v;

    // Construtor com valores por defeito (O truque mágico!)
    // Se passares só 3 valores, os outros assumem 0.
    Point(float x=0, float y=0, float z=0, 
          float nx=0, float ny=0, float nz=0, 
          float u=0, float v=0) 
        : x(x), y(y), z(z), nx(nx), ny(ny), nz(nz), u(u), v(v) {}
};

#endif