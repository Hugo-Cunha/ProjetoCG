#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <filesystem>

namespace fs = std::filesystem;

// --- Classes de Suporte ---
class Point {
public:
    float x, y, z;
    Point(float x, float y, float z) : x(x), y(y), z(z) {}
};

class Figure {
protected:
    std::vector<Point> vertices;
public:
    void addTriangle(Point p1, Point p2, Point p3) {
        vertices.push_back(p1);
        vertices.push_back(p2);
        vertices.push_back(p3);
    }
    void save(const std::string& filename) {
        std::string folder = "output";
        if (!fs::exists(folder)) {
            fs::create_directory(folder);
        }
        std::string path = folder + "/" + filename;

        std::ofstream file(path);
        if (file.is_open()) {
            file << vertices.size() << "\n";
            for (const auto& v : vertices) {
                file << v.x << " " << v.y << " " << v.z << "\n";
            }
            file.close();
            std::cout << "Sucesso: Ficheiro guardado em " << path << std::endl;
        } else {
            std::cerr << "Erro: Nao foi possivel criar o ficheiro em " << path << std::endl;
        }
    }
};

// --- Protótipos das Primitivas (Vamos implementar os "outros" a seguir) ---
class Plane : public Figure { public:
    Plane(float size, int divisions) {
        float start = -size / 2.0f;           // Início no canto negativo
        float step = size / (float)divisions; // Tamanho de cada quadrado da subdivisão

        for (int i = 0; i < divisions; ++i) {
            for (int j = 0; j < divisions; ++j) {
                // Coordenadas dos vértices da subdivisão atual (i, j)
                float x1 = start + (float)i * step;
                float x2 = start + (float)(i + 1) * step;
                float z1 = start + (float)j * step;
                float z2 = start + (float)(j + 1) * step;

                addTriangle(Point(x1, 0, z1), Point(x1, 0, z2), Point(x2, 0, z2));
                addTriangle(Point(x1, 0, z1), Point(x2, 0, z2), Point(x2, 0, z1));
            }
        }
    }
};

class Box : public Figure { public:
    Box(float size, int divisions) {
        float half = size / 2.0f;
        float step = size / (float)divisions;

        for (int i = 0; i < divisions; i++) {
            // 12 triangulos por cubo * o numero de divisions
            for (int j = 0; j < divisions; j++) {
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
                addTriangle(Point(a, c, half), Point(b, d, half), Point(a, d, half));
                addTriangle(Point(a, c, half), Point(b, c, half), Point(b, d, half));

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
    }
};

// Para a esfera, usamos coordenadas esféricas. Precisamos de percorrer a latitude (β ou phi) e a longitude (α ou theta).
// x=r⋅cos(β)⋅sin(α)
// y=r⋅sin(β)
// z=r⋅cos(β)⋅cos(α)
class Sphere : public Figure { public:
    Sphere(float radius, int slices, int stacks) {
        float alpha_step = (2.0f * M_PI) / slices;
        float beta_step = M_PI / stacks;

        for (int i = 0; i < slices; i++) {
            float a1 = i * alpha_step;
            float a2 = (i + 1) * alpha_step;

            for (int j = 0; j < stacks; j++) {
                // b vai de -PI/2 (fundo) até PI/2 (topo)
                float b1 = -M_PI / 2.0f + j * beta_step;
                float b2 = -M_PI / 2.0f + (j + 1) * beta_step;

                // 4 pontos que formam um "quadrado" na superfície da esfera
                Point p1(radius * cos(b1) * sin(a1), radius * sin(b1), radius * cos(b1) * cos(a1));
                Point p2(radius * cos(b1) * sin(a2), radius * sin(b1), radius * cos(b1) * cos(a2));
                Point p3(radius * cos(b2) * sin(a1), radius * sin(b2), radius * cos(b2) * cos(a1));
                Point p4(radius * cos(b2) * sin(a2), radius * sin(b2), radius * cos(b2) * cos(a2));

                // Triângulos para cada face da stack
                if (j != stacks - 1) addTriangle(p1, p2, p4);
                if (j != 0) addTriangle(p1, p4, p3);
            }
        }
    }
};

class Cone : public Figure { public:
    Cone(float radius, float height, int slices, int stacks) {
        float alpha_step = (2.0f * M_PI) / slices;
        float h_step = height / (float)stacks;
        float r_step = radius / (float)stacks;

        for (int i = 0; i < slices; i++) {
            float a1 = i * alpha_step;
            float a2 = (i + 1) * alpha_step;

            // 1. Base (plano XZ, y=0) [cite: 221]
            addTriangle(Point(0, 0, 0), 
                        Point(radius * sin(a2), 0, radius * cos(a2)),
                        Point(radius * sin(a1), 0, radius * cos(a1)));

            // 2. Lados (percorrendo as stacks)
            for (int j = 0; j < stacks; j++) {
                float h1 = j * h_step;
                float h2 = (j + 1) * h_step;
                float r1 = radius - (j * r_step);
                float r2 = radius - ((j + 1) * r_step);

                Point p1(r1 * sin(a1), h1, r1 * cos(a1));
                Point p2(r1 * sin(a2), h1, r1 * cos(a2));
                Point p3(r2 * sin(a1), h2, r2 * cos(a1));
                Point p4(r2 * sin(a2), h2, r2 * cos(a2));

                // Triângulos laterais
                addTriangle(p1, p2, p4);
                addTriangle(p1, p4, p3);
            }
        }
    } 
};

// --- Main Limpa e Organizada ---
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Uso: ./generator <figura> <parametros> <ficheiro.3d>" << std::endl;
        return 1;
    }

    std::string model = argv[1];
    Figure* fig = nullptr;

    try {
        if (model == "plane" && argc == 5) {
            // generator plane 1 3 plane.3d 
            fig = new Plane(std::stof(argv[2]), std::stoi(argv[3]));
            fig->save(argv[4]);
        } 
        else if (model == "box" && argc == 5) {
            // generator box 2 3 box.3d 
            fig = new Box(std::stof(argv[2]), std::stoi(argv[3]));
            fig->save(argv[4]);
        } 
        else if (model == "sphere" && argc == 6) {
            // generator sphere 1 10 10 sphere.3d 
            fig = new Sphere(std::stof(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]));
            fig->save(argv[5]);
        } 
        else if (model == "cone" && argc == 7) {
            // generator cone 1 2 4 3 cone.3d 
            fig = new Cone(std::stof(argv[2]), std::stof(argv[3]), std::stoi(argv[4]), std::stoi(argv[5]));
            fig->save(argv[6]);
        } 
        else {
            std::cout << "Comando invalido ou falta de argumentos." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Erro nos parametros: " << e.what() << std::endl;
    }

    delete fig;
    return 0;
}