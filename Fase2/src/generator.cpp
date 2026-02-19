#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <filesystem>
#include "utils/Figure.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Uso: ./generator <figura> <parametros> <ficheiro.3d>" << std::endl;
        return 1;
    }

    std::string model = argv[1];
    Figure *fig = nullptr;

    try{
        if (model == "plane" && argc == 5)  fig = new Plane(std::stof(argv[2]), std::stoi(argv[3]));

        else if (model == "box" && argc == 5)fig = new Box(std::stof(argv[2]), std::stoi(argv[3]));

        else if (model == "sphere" && argc == 6)fig = new Sphere(std::stof(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]));

        else if (model == "cone" && argc == 7)fig = new Cone(std::stof(argv[2]), std::stof(argv[3]), std::stoi(argv[4]), std::stoi(argv[5]));
        
        else if (model == "ring" && argc == 6) {fig = new Ring(std::stof(argv[2]), std::stof(argv[3]), std::stoi(argv[4]));

        }else{
            std::cout << "Comando invalido ou falta de argumentos." << std::endl;
        }
    }
    catch (const std::exception &e){
        std::cerr << "Erro nos parametros: " << e.what() << std::endl;
    }
    if (fig){
        fig->save(argv[argc-1]);
        delete fig;
    }
    return 0;
}