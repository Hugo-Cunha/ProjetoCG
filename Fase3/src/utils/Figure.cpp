#include <GL/glew.h>

#include "Figure.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <random>


namespace fs = std::filesystem;

float randomFloat(float a, float b)
{
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(a, b);
    return dist(rng);
}

Point scalePoint(const Point &p, float factor)
{
    return Point(
        p.x * factor,
        p.y * factor,
        p.z * factor);
}

void Figure::addTriangle(Point p1, Point p2, Point p3)
{
    vertices.push_back(p1);
    vertices.push_back(p2);
    vertices.push_back(p3);
}

void Figure::save(const std::string &filename)
{
    std::string folder = "output";
    if (!fs::exists(folder))
        fs::create_directory(folder);

    std::string path = folder + "/" + filename;
    std::ofstream file(path);

    if (file.is_open())
    {
        file << vertices.size() << "\n";
        for (const auto &v : vertices)
        {
            file << v.x << " " << v.y << " " << v.z << "\n";
        }
        file.close();
        std::cout << "Sucesso: Guardado em " << path << " (" << vertices.size() << " vertices)" << std::endl;
    }
}

bool Figure::load(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        return false;

    int numVertices;
    file >> numVertices;
    float x, y, z;
    for (int i = 0; i < numVertices; i++)
    {
        file >> x >> y >> z;
        vertices.push_back(Point(x, y, z));
    }
    file.close();
    return true;
}
/*
void Figure::draw()
{
    glBegin(GL_TRIANGLES);

    for (size_t i = 0; i + 2 < vertices.size(); i += 3)
    {
        int tri = (i / 3) % 2;

        switch (tri)
        {
        case 0:
            glColor3f(0.0f, 0.75f, 1.0f);
            break;
        case 1:
            glColor3f(1.0f, 0.55f, 0.2f);
            break;
        }

        glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
        glVertex3f(vertices[i + 1].x, vertices[i + 1].y, vertices[i + 1].z);
        glVertex3f(vertices[i + 2].x, vertices[i + 2].y, vertices[i + 2].z);
    }

    glEnd();
}
*/

void Figure::prepareVBO() {
    if (vertices.empty()) return;

    vertexCount = vertices.size();

    // 1. Criar o buffer
    glGenBuffers(1, &vbo);
    
    // 2. "Agarrar" o buffer para trabalhar nele
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    // 3. Copiar os dados do nosso vector para a memória da placa gráfica
    glBufferData(GL_ARRAY_BUFFER, 
                 sizeof(Point) * vertexCount, 
                 vertices.data(), 
                 GL_STATIC_DRAW);
    
    // 4. Limpar o vetor da RAM (opcional) para poupar memória, pois já está na GPU
    vertices.clear(); 
}

void Figure::draw() {
    if (vbo == 0) return; // Se não houver VBO, não desenha nada

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Dizemos ao OpenGL onde estão os floats (x, y, z) dentro do buffer
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    // O comando "mágico" que desenha tudo de uma vez 
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

Sphere::Sphere(float radius, int slices, int stacks)
{
    float alpha_step = (2.0f * M_PI) / slices;
    float beta_step = M_PI / stacks;

    int halfStacks = stacks / 2;
    bool isEven = (stacks % 2 == 0);

    // Função auxiliar para converter ângulos em Pontos
    auto getPoint = [radius](float alpha, float beta)
    {
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

        if (isEven)
        {
            bUp = bDown = M_PI / 2.0f; // Começa no equador (90 graus)
        }
        else
        {
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

        for (int j = 0; j < halfStacks - 1; ++j)
        {
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
Cone::Cone(float radius, float height, int slices, int stacks)
{
    float alpha_step = (2.0f * M_PI) / slices;
    float h_step = height / (float)stacks;

    // 1º CICLO: BASE (Círculo no chão Y=0)
    for (int i = 0; i < slices; ++i)
    {
        float a1 = i * alpha_step;
        float a2 = (i + 1) * alpha_step;

        Point p1(radius * sin(a1), 0, radius * cos(a1));
        Point p2(radius * sin(a2), 0, radius * cos(a2));
        Point center(0, 0, 0);

        // Ordem para ser visível de baixo (Sentido Anti-Horário/CCW)
        addTriangle(center, p2, p1);
    }

    // 2º CICLO: LADOS (Percorrer cada fatia/slice)
    for (int j = 0; j < slices; ++j)
    {
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
        for (int i = 0; i < stacks - 1; ++i)
        {
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

Ring::Ring(float ri, float re, int slices)
{
    float delta = (2.0f * M_PI) / slices;

    for (int i = 0; i < slices; i++)
    {
        float a1 = i * delta;
        float a2 = (i + 1) * delta;

        // Pontos do raio interno
        Point p1i(ri * sin(a1), 0.0f, ri * cos(a1));
        Point p2i(ri * sin(a2), 0.0f, ri * cos(a2));

        // Pontos do raio externo
        Point p1e(re * sin(a1), 0.0f, re * cos(a1));
        Point p2e(re * sin(a2), 0.0f, re * cos(a2));

        // --- FACE DE CIMA (Visível de Y > 0) ---
        // Triângulo 1: p1i, p2e, p1e
        addTriangle(p1i, p2e, p1e);
        // Triângulo 2: p1i, p2i, p2e
        addTriangle(p1i, p2i, p2e);

        // --- FACE DE BAIXO (Visível de Y < 0) ---
        // Invertemos a ordem (p1e em vez de p1i primeiro) para manter o CCW
        addTriangle(p1i, p1e, p2e);
        addTriangle(p1i, p2e, p2i);
    }
}
/*
Asteroid::Asteroid(float radius, int slices, int stacks, float roughness)
{
    float alpha_step = (2.0f * M_PI) / slices;
    float beta_step = M_PI / stacks;

    int halfStacks = stacks / 2;
    bool isEven = (stacks % 2 == 0);
    float randomvalor = 0.0f;
    float fator = 0.0f;
    // Função auxiliar para converter ângulos em Pontos
    auto getPoint = [radius](float alpha, float beta)
    {
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

        if (isEven)
        {
            bUp = bDown = M_PI / 2.0f; // Começa no equador (90 graus)
        }
        else
        {
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

        for (int j = 0; j < halfStacks - 1; ++j)
        {

            // METADE SUPERIOR (Rumo ao Polo Norte)
            float nextBUp = bUp - beta_step;

            Point nextP1Up = scalePoint(getPoint(a1, nextBUp), 1.0f + randomFloat(-roughness, roughness));
            Point nextP2Up = scalePoint(getPoint(a2, nextBUp), 1.0f + randomFloat(-roughness, roughness));

            addTriangle(p1Up, p2Up, nextP1Up);
            addTriangle(nextP1Up, p2Up, nextP2Up);

            bUp = nextBUp;
            p1Up = nextP1Up;
            p2Up = nextP2Up;

            bUp = nextBUp;
            p1Up = getPoint(a1, bUp);
            p2Up = getPoint(a2, bUp);

            // METADE INFERIOR (Rumo ao Polo Sul)
            float nextBDown = bDown + beta_step;

            Point nextP1Down = scalePoint(getPoint(a1, nextBDown), 1.0f + randomFloat(-roughness, roughness));
            Point nextP2Down = scalePoint(getPoint(a2, nextBDown), 1.0f + randomFloat(-roughness, roughness));

            addTriangle(p2Down, p1Down, nextP2Down);
            addTriangle(nextP2Down, p1Down, nextP1Down);

            bDown = nextBDown;
            p1Down = nextP1Down;
            p2Down = nextP2Down;
        }

        // Caps
        float fx = randomFloat(-roughness, roughness);
        float fy = randomFloat(-roughness, roughness);
        float fz = randomFloat(-roughness, roughness);

        Point norte(
            radius * fx,
            radius * (1.0f + fy),
            radius * fz);

        Point sul(
            radius * fx,
            -radius * (1.0f + fy),
            radius * fz);
        addTriangle(p1Up, p2Up, norte);   // Norte
        addTriangle(p2Down, p1Down, sul); // Sul
    }
};

*/
Asteroid::Asteroid(float radius, int slices, int stacks, float roughness)
{
    float alpha_step = (2.0f * M_PI) / slices;
    float beta_step = M_PI / stacks;

    int totalPoints = (stacks + 1) * (slices + 1);

    float *px = new float[totalPoints];
    float *py = new float[totalPoints];
    float *pz = new float[totalPoints];

    // 1. gerar todos os pontos primeiro
    for (int j = 0; j <= stacks; j++)
    {
        float beta = j * beta_step;
        float poleDeform = 1.0f + randomFloat(-roughness, roughness);

        for (int i = 0; i <= slices; i++)
        {
            float alpha = i * alpha_step;
            if (i == slices)
                alpha = 0.0f;

            float deform;
            if (j == 0 || j == stacks)
                deform = poleDeform;
            else
                deform = 1.0f + randomFloat(-roughness, roughness);

            float localRadius = radius * deform;

            int idx = j * (slices + 1) + i;

            px[idx] = localRadius * sin(beta) * sin(alpha);
            py[idx] = localRadius * cos(beta);
            pz[idx] = localRadius * sin(beta) * cos(alpha);
        }
    }

    // 2. construir os triângulos
    for (int j = 0; j < stacks; j++)
    {
        for (int i = 0; i < slices; i++)
        {
            int i1 = j * (slices + 1) + i;
            int i2 = j * (slices + 1) + (i + 1);
            int i3 = (j + 1) * (slices + 1) + i;
            int i4 = (j + 1) * (slices + 1) + (i + 1);

            Point p1(px[i1], py[i1], pz[i1]);
            Point p2(px[i2], py[i2], pz[i2]);
            Point p3(px[i3], py[i3], pz[i3]);
            Point p4(px[i4], py[i4], pz[i4]);

            addTriangle(p1, p2, p3);
            addTriangle(p3, p2, p4);
        }
    }

    delete[] px;
    delete[] py;
    delete[] pz;
};

float BezierPatch::bernstein(int i, float t) {
    switch (i) {
        case 0: return pow(1 - t, 3);
        case 1: return 3 * t * pow(1 - t, 2);
        case 2: return 3 * pow(t, 2) * (1 - t);
        case 3: return pow(t, 3);
        default: return 0;
    }
}

Point BezierPatch::getBezierPoint(float u, float v, const std::vector<Point>& controlPoints, const std::vector<int>& indices) {
    float x = 0, y = 0, z = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float b_u = bernstein(j, u); 
            float b_v = bernstein(i, v); 
            
            Point p = controlPoints[indices[i * 4 + j]];
            x += p.x * b_u * b_v;
            y += p.y * b_u * b_v;
            z += p.z * b_u * b_v;
        }
    }
    return Point(x, y, z);
}
BezierPatch::BezierPatch(const std::string &patchFile, int tessellation) {
    std::ifstream file(patchFile);
    if (!file.is_open()) return;

    std::string line;
    // 1. Ler número de patches
    std::getline(file, line);
    int numPatches = std::stoi(line);
    std::vector<std::vector<int>> allIndices(numPatches, std::vector<int>(16));

    for (int i = 0; i < numPatches; i++) {
        std::getline(file, line);
        std::stringstream ss(line);
        for (int j = 0; j < 16; j++) {
            std::string val;
            std::getline(ss, val, ',');
            allIndices[i][j] = std::stoi(val);
        }
    }

    // 2. Ler número de pontos de controlo e os pontos
    std::getline(file, line);
    int numControlPoints = std::stoi(line);
    std::vector<Point> controlPoints;
    for (int i = 0; i < numControlPoints; i++) {
        std::getline(file, line);
        std::stringstream ss(line);
        float x, y, z;
        std::string val;
        std::getline(ss, val, ','); x = std::stof(val);
        std::getline(ss, val, ','); y = std::stof(val);
        std::getline(ss, val, ','); z = std::stof(val);
        controlPoints.push_back(Point(x, y, z));
    }

    // 3. Gerar a malha de triângulos baseada na tesselação [cite: 120
    float step = 1.0f / tessellation;
    for (int p = 0; p < numPatches; p++) {
        for (int i = 0; i < tessellation; i++) {
            for (int j = 0; j < tessellation; j++) {
                float u1 = i * step, u2 = (i + 1) * step;
                float v1 = j * step, v2 = (j + 1) * step;

                Point p00 = getBezierPoint(u1, v1, controlPoints, allIndices[p]);
                Point p10 = getBezierPoint(u2, v1, controlPoints, allIndices[p]);
                Point p01 = getBezierPoint(u1, v2, controlPoints, allIndices[p]);
                Point p11 = getBezierPoint(u2, v2, controlPoints, allIndices[p]);

                // Adiciona os triângulos no sentido Anti-Horário (CCW)
                addTriangle(p00, p10, p01);
                addTriangle(p10, p11, p01);
            }
        }
    }
}