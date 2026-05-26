#include <GL/glew.h>

#include "Figure.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <sstream>
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
        for (const auto& v : vertices) {
            file << v.x << " " << v.y << " " << v.z << " " 
                << v.nx << " " << v.ny << " " << v.nz << " " 
                << v.u << " " << v.v << "\n";
        }
        file.close();
        std::cout << "Sucesso: Guardado em " << path << " (" << vertices.size() << " vertices)" << std::endl;
    }
}

bool Figure::load(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    int numVertices;
    file >> numVertices;
    float x, y, z, nx, ny, nz, u, v;
    for (int i = 0; i < numVertices; i++)
    {
        // Agora lemos os 8 valores de cada linha
        file >> x >> y >> z >> nx >> ny >> nz >> u >> v;
        vertices.push_back(Point(x, y, z, nx, ny, nz, u, v));
    }
    file.close();
    return true;
}

void Figure::prepareVBO() {
    if (vertices.empty()) return;
    vertexCount = vertices.size();

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Reparar que agora o sizeof(Point) é maior (8 floats em vez de 3)
    glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * vertexCount, vertices.data(), GL_STATIC_DRAW);
    vertices.clear(); 
}

void Figure::draw() {
    if (vbo == 0) return;

    // --- NOVIDADE FASE 4: MATERIAIS ---
    // Dizemos ao OpenGL como este objeto específico reage à luz
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, emissive);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    // ----------------------------------

    if (texID != 0) {
        glBindTexture(GL_TEXTURE_2D, texID);
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(Point), (void*)0);
    glNormalPointer(GL_FLOAT, sizeof(Point), (void*)(3 * sizeof(float)));
    glTexCoordPointer(2, GL_FLOAT, sizeof(Point), (void*)(6 * sizeof(float)));

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// --- Protótipos das Primitivas
Plane::Plane(float size, int divisions) {
    float start = -size / 2.0f;
    float step = size / (float)divisions;

    for (int i = 0; i < divisions; ++i) {
        for (int j = 0; j < divisions; ++j) {
            float x1 = start + i * step, z1 = start + j * step;
            float x2 = start + (i + 1) * step, z2 = start + (j + 1) * step;

            // Texturas: de 0 a 1 ao longo da grelha
            float u1 = (float)i / divisions, v1 = (float)j / divisions;
            float u2 = (float)(i + 1) / divisions, v2 = (float)(j + 1) / divisions;

            // Normal do plano aponta sempre para cima (0, 1, 0)
            Point p1(x1, 0, z1,  0, 1, 0,  u1, v1);
            Point p2(x1, 0, z2,  0, 1, 0,  u1, v2);
            Point p3(x2, 0, z2,  0, 1, 0,  u2, v2);
            Point p4(x2, 0, z1,  0, 1, 0,  u2, v1);

            addTriangle(p1, p2, p3);
            addTriangle(p1, p3, p4);
        }
    }
};

Box::Box(float size, int divisions) {
    float half = size / 2.0f;
    float step = size / (float)divisions;

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            float p1 = -half + i * step;
            float p2 = -half + (i + 1) * step;
            float p3 = -half + j * step;
            float p4 = -half + (j + 1) * step;

            float u1 = (float)i / divisions, v1 = (float)j / divisions;
            float u2 = (float)(i + 1) / divisions, v2 = (float)(j + 1) / divisions;

            // Front (+Z)
            addTriangle(Point(p1, p3, half, 0,0,1, u1,v1), Point(p2, p3, half, 0,0,1, u2,v1), Point(p2, p4, half, 0,0,1, u2,v2));
            addTriangle(Point(p1, p3, half, 0,0,1, u1,v1), Point(p2, p4, half, 0,0,1, u2,v2), Point(p1, p4, half, 0,0,1, u1,v2));

            // Back (-Z) -> X é invertido para virar as normais para trás
            addTriangle(Point(p2, p3, -half, 0,0,-1, u2,v1), Point(p1, p3, -half, 0,0,-1, u1,v1), Point(p1, p4, -half, 0,0,-1, u1,v2));
            addTriangle(Point(p2, p3, -half, 0,0,-1, u2,v1), Point(p1, p4, -half, 0,0,-1, u1,v2), Point(p2, p4, -half, 0,0,-1, u2,v2));

            // Right (+X) 
            addTriangle(Point(half, p3, p2, 1,0,0, u2,v1), Point(half, p3, p1, 1,0,0, u1,v1), Point(half, p4, p1, 1,0,0, u1,v2));
            addTriangle(Point(half, p3, p2, 1,0,0, u2,v1), Point(half, p4, p1, 1,0,0, u1,v2), Point(half, p4, p2, 1,0,0, u2,v2));

            // Left (-X)
            addTriangle(Point(-half, p3, p1, -1,0,0, u1,v1), Point(-half, p3, p2, -1,0,0, u2,v1), Point(-half, p4, p2, -1,0,0, u2,v2));
            addTriangle(Point(-half, p3, p1, -1,0,0, u1,v1), Point(-half, p4, p2, -1,0,0, u2,v2), Point(-half, p4, p1, -1,0,0, u1,v2));

            // Top (+Y) 
            addTriangle(Point(p1, half, p4, 0,1,0, u1,v2), Point(p2, half, p4, 0,1,0, u2,v2), Point(p2, half, p3, 0,1,0, u2,v1));
            addTriangle(Point(p1, half, p4, 0,1,0, u1,v2), Point(p2, half, p3, 0,1,0, u2,v1), Point(p1, half, p3, 0,1,0, u1,v1));

            // Bottom (-Y)
            addTriangle(Point(p1, -half, p3, 0,-1,0, u1,v1), Point(p2, -half, p3, 0,-1,0, u2,v1), Point(p2, -half, p4, 0,-1,0, u2,v2));
            addTriangle(Point(p1, -half, p3, 0,-1,0, u1,v1), Point(p2, -half, p4, 0,-1,0, u2,v2), Point(p1, -half, p4, 0,-1,0, u1,v2));
        }
    }
};

// Para a esfera, usamos coordenadas esféricas. Precisamos de percorrer a latitude (β ou phi) e a longitude (α ou theta).
// x= r⋅cos(β)⋅sin(α)
// y= r⋅sin(β)
// z= r⋅cos(β)⋅cos(α)


/*
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
*/

Sphere::Sphere(float radius, int slices, int stacks) {
    float alpha_step = (2 * M_PI) / slices;
    float beta_step = M_PI / stacks;

    for (int i = 0; i < slices; i++) {
        float alpha = i * alpha_step;
        float alpha_next = (i + 1) * alpha_step;
        
        // Coordenada U da textura (eixo X da imagem)
        float u1 = 1.0f - ((float)i / slices);
        float u2 = 1.0f - ((float)(i + 1) / slices);

        for (int j = 0; j < stacks; j++) {
            float beta = (j * beta_step) - (M_PI / 2);
            float beta_next = ((j + 1) * beta_step) - (M_PI / 2);

            // Coordenada V da textura (eixo Y da imagem)
            float v1 = (float)j / stacks;
            float v2 = (float)(j + 1) / stacks;

            auto makePoint = [radius](float a, float b, float u, float v) {
                float x = radius * cos(b) * sin(a);
                float y = radius * sin(b);
                float z = radius * cos(b) * cos(a);
                // A normal na esfera é só o vetor Posição normalizado (dividido pelo raio)
                return Point(x, y, z, x/radius, y/radius, z/radius, u, v);
            };

            Point p1 = makePoint(alpha, beta, u1, v1);
            Point p2 = makePoint(alpha_next, beta, u2, v1);
            Point p3 = makePoint(alpha, beta_next, u1, v2);
            Point p4 = makePoint(alpha_next, beta_next, u2, v2);

            addTriangle(p1, p2, p3);
            addTriangle(p2, p4, p3);
        }
    }
}

// Logica com 3 ciclos para os triangulos nao ficare tortos
Cone::Cone(float radius, float height, int slices, int stacks) {
    float alpha_step = (2.0f * M_PI) / slices;
    float h_step = height / (float)stacks;

    // Normal das laterais do cone (calculada com a inclinação)
    float ny = radius / sqrt(radius*radius + height*height);

    for (int i = 0; i < slices; ++i) {
        float a1 = i * alpha_step;
        float a2 = (i + 1) * alpha_step;
        
        // 1. BASE (Normal 0, -1, 0)
        float u1_base = 0.5f + 0.5f * sin(a1), v1_base = 0.5f + 0.5f * cos(a1);
        float u2_base = 0.5f + 0.5f * sin(a2), v2_base = 0.5f + 0.5f * cos(a2);
        
        Point base_center(0, 0, 0,  0, -1, 0,  0.5f, 0.5f);
        Point base_p1(radius * sin(a1), 0, radius * cos(a1),  0, -1, 0,  u1_base, v1_base);
        Point base_p2(radius * sin(a2), 0, radius * cos(a2),  0, -1, 0,  u2_base, v2_base);
        addTriangle(base_center, base_p2, base_p1); // CCW virado para baixo

        // 2. LADOS
        float nx1 = sin(a1) * (height / sqrt(radius*radius + height*height));
        float nz1 = cos(a1) * (height / sqrt(radius*radius + height*height));
        float nx2 = sin(a2) * (height / sqrt(radius*radius + height*height));
        float nz2 = cos(a2) * (height / sqrt(radius*radius + height*height));

        Point current_p1(radius * sin(a1), 0, radius * cos(a1), nx1, ny, nz1, (float)i/slices, 0);
        Point current_p2(radius * sin(a2), 0, radius * cos(a2), nx2, ny, nz2, (float)(i+1)/slices, 0);

        float dr1x = current_p1.x / stacks, dr1z = current_p1.z / stacks;
        float dr2x = current_p2.x / stacks, dr2z = current_p2.z / stacks;

        for (int j = 0; j < stacks - 1; ++j) {
            float v_next = (float)(j+1)/stacks;
            Point next_p1(current_p1.x - dr1x, current_p1.y + h_step, current_p1.z - dr1z, nx1, ny, nz1, (float)i/slices, v_next);
            Point next_p2(current_p2.x - dr2x, current_p2.y + h_step, current_p2.z - dr2z, nx2, ny, nz2, (float)(i+1)/slices, v_next);

            addTriangle(current_p1, current_p2, next_p1);
            addTriangle(current_p2, next_p2, next_p1);

            current_p1 = next_p1;
            current_p2 = next_p2;
        }

        // Topo (Apex)
        Point apex1(0, height, 0, nx1, ny, nz1, (float)i/slices, 1);
        Point apex2(0, height, 0, nx2, ny, nz2, (float)(i+1)/slices, 1);
        addTriangle(current_p1, current_p2, apex1);
    }
}

Ring::Ring(float ri, float re, int slices) {
    float delta = (2.0f * M_PI) / slices;

    for (int i = 0; i < slices; i++) {
        float a1 = i * delta;
        float a2 = (i + 1) * delta;

        float v1 = (float)i / slices;
        float v2 = (float)(i + 1) / slices;

        float ui = 1.0f; // raio interior
        float ue = 0.0f; // raio exterior

        // FACE DE CIMA
        Point top_p1i(ri * sin(a1), 0, ri * cos(a1),  0, 1, 0,  ui, v1);
        Point top_p2i(ri * sin(a2), 0, ri * cos(a2),  0, 1, 0,  ui, v2);
        Point top_p1e(re * sin(a1), 0, re * cos(a1),  0, 1, 0,  ue, v1);
        Point top_p2e(re * sin(a2), 0, re * cos(a2),  0, 1, 0,  ue, v2);

        addTriangle(top_p1i, top_p2e, top_p1e);
        addTriangle(top_p1i, top_p2i, top_p2e);

        // FACE DE BAIXO
        Point bot_p1i(ri * sin(a1), 0, ri * cos(a1),  0, -1, 0,  ui, v1);
        Point bot_p2i(ri * sin(a2), 0, ri * cos(a2),  0, -1, 0,  ui, v2);
        Point bot_p1e(re * sin(a1), 0, re * cos(a1),  0, -1, 0,  ue, v1);
        Point bot_p2e(re * sin(a2), 0, re * cos(a2),  0, -1, 0,  ue, v2);

        addTriangle(bot_p1i, bot_p1e, bot_p2e);
        addTriangle(bot_p1i, bot_p2e, bot_p2i);
    }
}

Asteroid::Asteroid(float radius, int slices, int stacks, float roughness) {
    float alpha_step = (2.0f * M_PI) / slices;
    float beta_step = M_PI / stacks;
    int totalPoints = (stacks + 1) * (slices + 1);

    std::vector<Point> astPoints;

    for (int j = 0; j <= stacks; j++) {
        float beta = (j * beta_step) - (M_PI / 2);
        float poleDeform = 1.0f + randomFloat(-roughness, roughness);
        float v = (float)j / stacks;

        for (int i = 0; i <= slices; i++) {
            float alpha = i * alpha_step;
            float u = (float)i / slices;

            float deform = (j == 0 || j == stacks) ? poleDeform : 1.0f + randomFloat(-roughness, roughness);
            float localRadius = radius * deform;

            float x = localRadius * cos(beta) * sin(alpha);
            float y = localRadius * sin(beta);
            float z = localRadius * cos(beta) * cos(alpha);

            // Aproximamos a normal pela direção do ponto desde o centro
            float length = sqrt(x*x + y*y + z*z);
            astPoints.push_back(Point(x, y, z, x/length, y/length, z/length, u, v));
        }
    }

    for (int j = 0; j < stacks; j++) {
        for (int i = 0; i < slices; i++) {
            int i1 = j * (slices + 1) + i;
            int i2 = j * (slices + 1) + (i + 1);
            int i3 = (j + 1) * (slices + 1) + i;
            int i4 = (j + 1) * (slices + 1) + (i + 1);

            addTriangle(astPoints[i1], astPoints[i2], astPoints[i3]);
            addTriangle(astPoints[i3], astPoints[i2], astPoints[i4]);
        }
    }
};

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

/*
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
*/

float BezierPatch::bernstein(int i, float t) {
    switch (i) {
        case 0: return pow(1 - t, 3);
        case 1: return 3 * t * pow(1 - t, 2);
        case 2: return 3 * pow(t, 2) * (1 - t);
        case 3: return pow(t, 3);
        default: return 0;
    }
}

float BezierPatch::bernsteinDeriv(int i, float t) {
    switch (i) {
        case 0: return -3.0f * pow(1.0f - t, 2.0f);
        case 1: return 3.0f * pow(1.0f - t, 2.0f) - 6.0f * t * (1.0f - t);
        case 2: return 6.0f * t * (1.0f - t) - 3.0f * pow(t, 2.0f);
        case 3: return 3.0f * pow(t, 2.0f);
        default: return 0;
    }
}

Point BezierPatch::getBezierPoint(float u, float v, const std::vector<Point>& controlPoints, const std::vector<int>& indices) {
    float x = 0, y = 0, z = 0;
    float du_x = 0, du_y = 0, du_z = 0; // Derivada U (Tangente horizontal)
    float dv_x = 0, dv_y = 0, dv_z = 0; // Derivada V (Tangente vertical)

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            Point p = controlPoints[indices[i * 4 + j]];
            
            // Valor normal
            float b_u = bernstein(j, u); 
            float b_v = bernstein(i, v); 
            // Derivadas
            float d_u = bernsteinDeriv(j, u);
            float d_v = bernsteinDeriv(i, v);

            // Posição
            x += p.x * b_u * b_v;
            y += p.y * b_u * b_v;
            z += p.z * b_u * b_v;

            // Tangentes
            du_x += p.x * d_u * b_v; du_y += p.y * d_u * b_v; du_z += p.z * d_u * b_v;
            dv_x += p.x * b_u * d_v; dv_y += p.y * b_u * d_v; dv_z += p.z * b_u * d_v;
        }
    }

    float nx = du_y * dv_z - du_z * dv_y;
    float ny = du_z * dv_x - du_x * dv_z;
    float nz = du_x * dv_y - du_y * dv_x;
    
    float length = sqrt(nx*nx + ny*ny + nz*nz);
    if(length > 0) { nx /= length; ny /= length; nz /= length; }

    return Point(x, y, z, nx, ny, nz, u, v);
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

    // 3. Gerar a malha de triângulos baseada na tesselação
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

void Figure::loadTextureToGPU() {
    // Se não houver ficheiro de textura, ignorar
    if (textureFile.empty()) return;

    unsigned int t, tw, th;
    unsigned char *texData;

    // 1. Gerar e ler a imagem com o DevIL
    ilGenImages(1, &t);
    ilBindImage(t);
    if (!ilLoadImage((ILstring)textureFile.c_str())) {
        std::cout << "Erro: Nao foi possivel carregar a textura: " << textureFile << std::endl;
        return;
    }

    tw = ilGetInteger(IL_IMAGE_WIDTH);
    th = ilGetInteger(IL_IMAGE_HEIGHT);
    
    // Converte para RGBA para garantir que o OpenGL percebe a cor
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    texData = ilGetData();

    // 2. Enviar a imagem para o OpenGL
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Dizer ao OpenGL como esticar/encolher a imagem (Filtros)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // O comando que envia mesmo os pixeis para a memória da gráfica!
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);

    // Desligar para não bugar
    glBindTexture(GL_TEXTURE_2D, 0);
}