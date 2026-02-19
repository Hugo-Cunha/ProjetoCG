#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <filesystem>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Figure.h"

namespace fs = std::filesystem;

void Figure::addTriangle(Point p1, Point p2, Point p3) {
    vertices.push_back(p1);
    vertices.push_back(p2);
    vertices.push_back(p3);
}

void Figure::save(const std::string& filename) {
    std::string folder = "output";
    if (!fs::exists(folder)) fs::create_directory(folder);
    
    std::string path = folder + "/" + filename;
    std::ofstream file(path);

    if (file.is_open()) {
        file << vertices.size() << "\n";
        for (const auto& v : vertices) {
            file << v.x << " " << v.y << " " << v.z << "\n";
        }
        file.close();
        std::cout << "Sucesso: Guardado em " << path << " (" << vertices.size() << " vertices)" << std::endl;
    }
}

bool Figure::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    int numVertices;
    file >> numVertices;
    float x, y, z;
    for (int i = 0; i < numVertices; i++) {
        file >> x >> y >> z;
        vertices.push_back(Point(x, y, z));
    }
    file.close();
    return true;
}

void Figure::draw(){
    glBegin(GL_TRIANGLES);
    for (const auto& p : vertices) {
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
}

// --- Protótipos das Primitivas
Plane::Plane(float size, int divisions)
    {
        float start = -size / 2.0f;           // Início no canto negativo
        float step = size / (float)divisions; // Tamanho de cada quadrado da subdivisão

        for (int i = 0; i < divisions; ++i)
        {
            for (int j = 0; j < divisions; ++j)
            {
                // Coordenadas dos vértices da subdivisão atual (i, j)
                float x1 = start + (float)i * step;
                float x2 = start + (float)(i + 1) * step;
                float z1 = start + (float)j * step;
                float z2 = start + (float)(j + 1) * step;

                addTriangle(Point(x1, 0, z1), Point(x1, 0, z2), Point(x2, 0, z2));
                addTriangle(Point(x1, 0, z1), Point(x2, 0, z2), Point(x2, 0, z1));
            }
        }
};


Box::
    Box(float size, int divisions)
    {
        float half = size / 2.0f;
        float step = size / (float)divisions;

        for (int i = 0; i < divisions; i++)
        {
            // 12 triangulos por cubo * o numero de divisions
            for (int j = 0; j < divisions; j++)
            {
                float a = -half + (float)i * step;
                float b = -half + (float)(i + 1) * step;
                float c = -half + (float)j * step;
                float d = -half + (float)(j + 1) * step;

                // Face de Cima (Y = half)
                addTriangle(Point(a, half, c), Point(a, half, d), Point(b, half, d));
                addTriangle(Point(a, half, c), Point(b, half, d), Point(b, half, c));

                // Face de Baixo (Y = -half)
                addTriangle(Point(a, -half, c), Point(b, -half, d), Point(a, -half, d));
                addTriangle(Point(a, -half, c), Point(b, -half, c), Point(b, -half, d));

                // Face Frontal (Z = half)
                addTriangle(Point(a, c, half), Point(a, d, half), Point(b, d, half));
                addTriangle(Point(a, c, half), Point(b, d, half), Point(b, c, half));

                // Face Traseira (Z = -half)
                addTriangle(Point(a, c, -half), Point(a, d, -half), Point(b, d, -half));
                addTriangle(Point(a, c, -half), Point(b, d, -half), Point(b, c, -half));

                // Face Lateral Direita (X = half)
                addTriangle(Point(half, a, c), Point(half, b, d), Point(half, a, d));
                addTriangle(Point(half, a, c), Point(half, b, c), Point(half, b, d));

                // Face Lateral Esquerda (X = -half)
                addTriangle(Point(-half, a, c), Point(-half, a, d), Point(-half, b, d));
                addTriangle(Point(-half, a, c), Point(-half, b, d), Point(-half, b, c));
            }
        }
};

// Para a esfera, usamos coordenadas esféricas. Precisamos de percorrer a latitude (β ou phi) e a longitude (α ou theta).
// x= r⋅cos(β)⋅sin(α)
// y= r⋅sin(β)
// z= r⋅cos(β)⋅cos(α)

Sphere::Sphere(float radius, int slices, int stacks){
        float alpha_step = (2.0f * M_PI) / slices;
        float beta_step = M_PI / stacks;

        int halfStacks = stacks / 2;
        bool isEven = (stacks % 2 == 0);

        // Função auxiliar para converter ângulos em Pontos
        auto getPoint = [radius](float alpha, float beta){
            return Point(radius * sin(beta) * sin(alpha),
                         radius * cos(beta),
                         radius * sin(beta) * cos(alpha));
        };
        // x= r⋅cos(β)⋅sin(α)
        // y= r⋅sin(β)
        // z= r⋅cos(β)⋅cos(α)

        for (int i = 0; i < slices; ++i)
        {
            float a1 = i * alpha_step;
            float a2 = (i + 1) * alpha_step;

            float bUp, bDown;

            if (isEven){
                bUp = bDown = M_PI / 2.0f; // Começa no equador (90 graus)
            }
            else{
                bUp = M_PI / 2.0f + (beta_step / 2.0f);
                bDown = M_PI / 2.0f - (beta_step / 2.0f);

                // Faixa do equador (ajuste para stacks ímpares)
                addTriangle(getPoint(a1, bDown), getPoint(a2, bDown), getPoint(a1, bUp));
                addTriangle(getPoint(a1, bUp), getPoint(a2, bDown), getPoint(a2, bUp));
            }

            // Pontos de controlo que "sobem" e "descem" a esfera
            Point p1Up = getPoint(a1, bUp);
            Point p2Up = getPoint(a2, bUp);
            Point p1Down = getPoint(a1, bDown);
            Point p2Down = getPoint(a2, bDown);

            for (int j = 0; j < halfStacks - 1; ++j){
                // METADE SUPERIOR (Rumo ao Polo Norte)
                float nextBUp = bUp - beta_step;
                addTriangle(p1Up, p2Up, getPoint(a1, nextBUp));
                addTriangle(getPoint(a1, nextBUp), p2Up, getPoint(a2, nextBUp));

                bUp = nextBUp;
                p1Up = getPoint(a1, bUp);
                p2Up = getPoint(a2, bUp);

                // METADE INFERIOR (Rumo ao Polo Sul)
                float nextBDown = bDown + beta_step;
                addTriangle(p2Down, p1Down, getPoint(a2, nextBDown));
                addTriangle(getPoint(a2, nextBDown), p1Down, getPoint(a1, nextBDown));

                bDown = nextBDown;
                p1Down = getPoint(a1, bDown);
                p2Down = getPoint(a2, bDown);
            }

            // Caps
            addTriangle(p1Up, p2Up, Point(0.0f, radius, 0.0f));      // Norte
            addTriangle(p2Down, p1Down, Point(0.0f, -radius, 0.0f)); // Sul
        }
    
};

// Logica com 3 ciclos para os triangulos nao ficare tortos
Cone::Cone(float radius, float height, int slices, int stacks){
        float alpha_step = (2.0f * M_PI) / slices;
        float h_step = height / (float)stacks;

        // 1º CICLO: BASE (Círculo no chão Y=0)
        for (int i = 0; i < slices; ++i) {
            float a1 = i * alpha_step;
            float a2 = (i + 1) * alpha_step;

            Point p1(radius * sin(a1), 0, radius * cos(a1));
            Point p2(radius * sin(a2), 0, radius * cos(a2));
            Point center(0, 0, 0);

            // Ordem para ser visível de baixo (Sentido Anti-Horário/CCW)
            addTriangle(center, p2, p1);
        }

        // 2º CICLO: LADOS (Percorrer cada fatia/slice)
        for (int j = 0; j < slices; ++j) {
            float a1 = j * alpha_step;
            float a2 = (j + 1) * alpha_step;

            // Pontos de partida na base da figura
            Point current_p1(radius * sin(a1), 0, radius * cos(a1));
            Point current_p2(radius * sin(a2), 0, radius * cos(a2));

            float dr1x = current_p1.x / stacks;
            float dr1z = current_p1.z / stacks;
            float dr2x = current_p2.x / stacks;
            float dr2z = current_p2.z / stacks;

            // 3º CICLO: STACKS (Subir no cone)
            for (int i = 0; i < stacks - 1; ++i) {
                // Calculamos os pontos da próxima "camada"
                Point next_p1(current_p1.x - dr1x, current_p1.y + h_step, current_p1.z - dr1z);
                Point next_p2(current_p2.x - dr2x, current_p2.y + h_step, current_p2.z - dr2z);

                // Triângulos laterais (CCW - Visíveis por fora)
                addTriangle(current_p1, current_p2, next_p1);
                addTriangle(current_p2, next_p2, next_p1);

                // Atualizamos os pontos para a próxima stack
                current_p1 = next_p1;
                current_p2 = next_p2;
            }

            // TOPO: O triângulo final que fecha no bico (Apex)
            addTriangle(current_p1, current_p2, Point(0, height, 0));
        }
};