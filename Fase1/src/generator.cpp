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
        vertices.push_back(p1);vertices.push_back(p2);vertices.push_back(p3);
    }
    void save(const std::string& filename) {
        fs::path folder{"build/output"};
        if (!fs::exists(folder)) {
            fs::create_directory(folder);
        }
        fs::path path = folder / filename;

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
    }
};

// Para a esfera, usamos coordenadas esféricas. Precisamos de percorrer a latitude (β ou phi) e a longitude (α ou theta).
// x= r⋅cos(β)⋅sin(α)
// y= r⋅sin(β)
// z= r⋅cos(β)⋅cos(α)
class Sphere : public Figure {
public:
    Sphere(float radius, int slices, int stacks) {
        float alpha_step = (2.0f * M_PI) / slices;
        float beta_step = M_PI / stacks;

        int halfStacks = stacks / 2;
        bool isEven = (stacks % 2 == 0);

        // Função auxiliar para converter ângulos em Pontos (mais limpo)
        auto getPoint = [radius](float alpha, float beta) {
            return Point(radius * sin(beta) * sin(alpha),
                         radius * cos(beta),
                         radius * sin(beta) * cos(alpha));
        };

        for (int i = 0; i < slices; ++i) {
            float a1 = i * alpha_step;
            float a2 = (i + 1) * alpha_step;

            float bUp, bDown;

            if (isEven) {
                bUp = bDown = M_PI / 2.0f; // Começa no equador (90 graus)
            } else {
                bUp = M_PI / 2.0f + (beta_step / 2.0f);
                bDown = M_PI / 2.0f - (beta_step / 2.0f);

                // Faixa do equador (ajuste para stacks ímpares)
                addTriangle(getPoint(a1, bDown), getPoint(a2, bDown), getPoint(a1, bUp));
                addTriangle(getPoint(a1, bUp),   getPoint(a2, bDown), getPoint(a2, bUp));
            }

            // Pontos de controlo que "sobem" e "descem" a esfera
            Point p1Up = getPoint(a1, bUp);
            Point p2Up = getPoint(a2, bUp);
            Point p1Down = getPoint(a1, bDown);
            Point p2Down = getPoint(a2, bDown);

            for (int j = 0; j < halfStacks - 1; ++j) {
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
            addTriangle(p1Up, p2Up, Point(0.0f, radius, 0.0f));       // Norte
            addTriangle(p2Down, p1Down, Point(0.0f, -radius, 0.0f));    // Sul
        }
    }
};

class Cone : public Figure {
public:
    Cone(float radius, float height, int slices, int stacks) {
        float alpha_step = (2.0f * M_PI) / slices;
        float h_step = height / (float)stacks;

        for (int i = 0; i < slices; ++i) {
            float a1 = i * alpha_step;
            float a2 = (i + 1) * alpha_step;

            // 1. Desenhar a Base (Círculo no chão Y=0)
            addTriangle(Point(0,0,0), 
                        Point(radius * sin(a2), 0, radius * cos(a2)),
                        Point(radius * sin(a1), 0, radius * cos(a1)));

            // 2. Desenhar as Laterais (Stacks)
            for (int j = 0; j < stacks; j++) {
                // r1 e r2 calculam o raio atual com base na altura (semelhança de triângulos)
                float r1 = radius * (float(stacks - j) / stacks);
                float r2 = radius * (float(stacks - (j + 1)) / stacks);
                
                float h1 = j * h_step;
                float h2 = (j + 1) * h_step;

                Point p1(r1 * sin(a1), h1, r1 * cos(a1));
                Point p2(r1 * sin(a2), h1, r1 * cos(a2));
                Point p3(r2 * sin(a1), h2, r2 * cos(a1));
                Point p4(r2 * sin(a2), h2, r2 * cos(a2));

                if (j == stacks - 1) {
                    // Última stack: Apenas um triângulo que fecha na ponta (Apex)
                    addTriangle(p1, p2, Point(0, height, 0));
                } else {
                    // Corpo: Dois triângulos formando um trapézio inclinado
                    addTriangle(p1, p2, p4);
                    addTriangle(p1, p4, p3);
                }
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
            fig = new Plane(std::stof(argv[2]), std::stoi(argv[3]));
            fig->save(argv[4]);
        } 
        else if (model == "box" && argc == 5) {
            fig = new Box(std::stof(argv[2]), std::stoi(argv[3]));
            fig->save(argv[4]);
        } 
        else if (model == "sphere" && argc == 6) {
            fig = new Sphere(std::stof(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]));
            fig->save(argv[5]);
        } 
        else if (model == "cone" && argc == 7) {
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