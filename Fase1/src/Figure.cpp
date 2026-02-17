#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <filesystem>

#include "Point.h"

namespace fs = std::filesystem;

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