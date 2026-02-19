#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include "utils/Figure.h"
#include "utils/Config.h"

using namespace std;

// Estrutura para a configuração da cena (Câmara + Modelos)

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
    for (auto& fig : sceneConfig.models) {
        // E chama o método draw() de cada uma
        fig.draw(); 
    }
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

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Uso: ./engine config.xml" << endl;
        return 1;
    }

    if (!sceneConfig.loadXML(argv[1])) {
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
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(keyboard);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glutMainLoop();
    return 0;
}