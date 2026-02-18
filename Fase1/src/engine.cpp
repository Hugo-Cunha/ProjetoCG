#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include "tinyXML/tinyxml2.h" // Usamos TinyXML2

using namespace std;

class Point {
public:
    float x, y, z;
    Point(float x, float y, float z) : x(x), y(y), z(z) {}
};

// Classe Figure expandida para carregar ficheiros
class Figure {
public:
    vector<Point> vertices;

    bool load(const string& filename) {
        ifstream file(filename);
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

    void addTriangle(Point p1, Point p2, Point p3) {
        vertices.push_back(p1);
        vertices.push_back(p2);
        vertices.push_back(p3);
    }
};

// Estrutura para a configuração da cena (Câmara + Modelos)
struct Config {
    float posCam[3], lookAt[3], up[3], projection[3];
    float fov, near, far;
    int width, height;
    vector<Figure> models;
};

Config sceneConfig;

float alpha, beta_, radius;

int mode = GL_LINE;

float posX = 0.0f, posZ = 0.0f;
float angle = 0.0f;
float scaleY = 1.0f;

// -----------------------------------------------

void changeSize(int w, int h) {
	if(h == 0)
		h = 1;
	float ratio = w * 1.0 / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glViewport(0, 0, w, h);

    gluPerspective(sceneConfig.fov, ratio, sceneConfig.near, sceneConfig.far);
	glMatrixMode(GL_MODELVIEW);
}


void drawModels() {
    glBegin(GL_TRIANGLES);
    for (const auto& fig : sceneConfig.models) {
        for (const auto& p : fig.vertices) {
            glVertex3f(p.x, p.y, p.z);
        }
    }
    glEnd();
}

void drawAxes(){
	glBegin(GL_LINES);
        // X axis in red
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-100.0f, 0.0f, 0.0f);
        glVertex3f( 100.0f, 0.0f, 0.0f);
        // Y Axis in Green
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, -100.0f, 0.0f);
        glVertex3f(0.0f, 100.0f, 0.0f);
        // Z Axis in Blue
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, -100.0f);
        glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();
}

void keyboard(unsigned char key, int x, int y){
	switch (key) {
		case 'a':
			angle -= 5.0f;
			break;
		case 'd':
			angle += 5.0f;
			break;
		case 'w':
			scaleY += 0.5f;
			break;
		case 's':
			scaleY -= 0.5f;
			break;
		case 'f':
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case 'l':
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case 'p':
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			break;
	}
	glutPostRedisplay();
}


void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Câmara usando coordenadas esféricas para permitir rotação
    float camX = radius * cos(beta_) * sin(alpha);
    float camY = radius * sin(beta_);
    float camZ = radius * cos(beta_) * cos(alpha);

    gluLookAt(camX, camY, camZ,
              sceneConfig.lookAt[0], sceneConfig.lookAt[1], sceneConfig.lookAt[2],
              sceneConfig.up[0], sceneConfig.up[1], sceneConfig.up[2]);

    // Desenho dos eixos
    drawAxes();

	glTranslatef(posX, 0.0f, posZ);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glScalef(1.0f, scaleY, 1.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, mode);
    drawModels();

    glutSwapBuffers();
}

// ------------------------------------------------

bool loadConfig(const char* xmlPath) {
    tinyxml2::XMLDocument doc;
    // No TinyXML-2, o LoadFile devolve um código de erro (0 ou XML_SUCCESS é sucesso)
    if (doc.LoadFile(xmlPath) != tinyxml2::XML_SUCCESS) {
        return false;
    }

    tinyxml2::XMLElement* root = doc.FirstChildElement("world");
    if (!root) return false;

    tinyxml2::XMLElement* win = root->FirstChildElement("window");
    if (win) {
        sceneConfig.width = win->IntAttribute("width");
        sceneConfig.height = win->IntAttribute("height");
    } else {
        sceneConfig.width = 800; sceneConfig.height = 800; // Default
    }

    tinyxml2::XMLElement* cam = root->FirstChildElement("camera");
    if (cam) {
        // Posição: Usamos FloatAttribute para ler o número diretamente
        tinyxml2::XMLElement* pos = cam->FirstChildElement("position");
        sceneConfig.posCam[0] = pos->FloatAttribute("x");
        sceneConfig.posCam[1] = pos->FloatAttribute("y");
        sceneConfig.posCam[2] = pos->FloatAttribute("z");

        // LookAt
        tinyxml2::XMLElement* lookAt = cam->FirstChildElement("lookAt");
        sceneConfig.lookAt[0] = lookAt->FloatAttribute("x");
        sceneConfig.lookAt[1] = lookAt->FloatAttribute("y");
        sceneConfig.lookAt[2] = lookAt->FloatAttribute("z");

        // Up
        tinyxml2::XMLElement* up = cam->FirstChildElement("up");
        sceneConfig.up[0] = up->FloatAttribute("x");
        sceneConfig.up[1] = up->FloatAttribute("y");
        sceneConfig.up[2] = up->FloatAttribute("z");

        // Projeção
        tinyxml2::XMLElement* proj = cam->FirstChildElement("projection");
        if (proj) {
        sceneConfig.fov = proj->FloatAttribute("fov");
        sceneConfig.near = proj->FloatAttribute("near");
        sceneConfig.far = proj->FloatAttribute("far");
    }
    }

    // Modelos
    tinyxml2::XMLElement* group = root->FirstChildElement("group");
    tinyxml2::XMLElement* models = group->FirstChildElement("models");
    if (models) {
        for (tinyxml2::XMLElement* m = models->FirstChildElement("model"); m; m = m->NextSiblingElement("model")) {
            Figure f;
            const char* filename = m->Attribute("file");
            if (filename && f.load(filename)) {
                sceneConfig.models.push_back(f);
            }
        }
    }
    return true;
}

// ------------------------------------------------

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Uso: ./engine config.xml" << endl;
        return 1;
    }

    if (!loadConfig(argv[1])) {
        cout << "Erro ao carregar XML!" << endl;
        return 1;
    }

    radius = sqrt(pow(sceneConfig.posCam[0], 2) + pow(sceneConfig.posCam[1], 2) + pow(sceneConfig.posCam[2], 2));
    beta_ = asin(sceneConfig.posCam[1] / radius);
    alpha = atan2(sceneConfig.posCam[0], sceneConfig.posCam[2]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

    glutInitWindowSize(sceneConfig.width, sceneConfig.height);
    glutCreateWindow("CG Engine - Fase 1");

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize); // Usa a tua função changeSize
    glutKeyboardFunc(keyboard);   // Registar as tuas funções de teclado

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glutMainLoop();
    return 0;
}