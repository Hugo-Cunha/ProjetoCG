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

Config sceneConfig;

float alpha, beta_, radius;

int mode = GL_LINE;
bool axes = true;

float posX = 0.0f, posZ = 0.0f;
float angle = 0.0f;
float scaleY = 1.0f;

const float cameraAngleStep = 3.14159265f / 30.0f;
const float cameraZoomStep = 0.5f;

Figure asteroid1;
Figure asteroid2;
Figure asteroid3;

// -----------------------------------------------

void changeSize(int w, int h)
{
    if (h == 0)
        h = 1;
    float ratio = w * 1.0 / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);

    gluPerspective(sceneConfig.fov, ratio, sceneConfig.near, sceneConfig.far);
    glMatrixMode(GL_MODELVIEW);
}

void drawModels()
{
    if (sceneConfig.root)
    {
        sceneConfig.root->draw(); // Inicia a cascatak
    }
    else
    {
        std::cout << "[Engine] sceneConfig.root is null" << std::endl;
    }
}

void drawAxes()
{
    glBegin(GL_LINES);
    // X axis in red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);
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

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'a':
        angle -= 5.0f;
        break;
    case 'd':
        angle += 5.0f;
        break;
    case 'e':
        axes = !axes;
        break;
    case 'w':
        scaleY += 0.5f;
        break;
    case 's':
        scaleY -= 0.5f;
        break;
    case 'f':
        mode = GL_FILL;
        break;
    case 'l':
        mode = GL_LINE;
        break;
    case 'p':
        mode = GL_POINT;
        break;
        break;
    case 'i':
        radius -= cameraZoomStep;
        if (radius < 0.1f)
            radius = 0.1f;
        break;
    case 'o':
        radius += cameraZoomStep;
        break;
    }
    glutPostRedisplay();
}

void processSpecialKeys(int key, int x, int y)
{
    const float betaLimit = (3.14159265f / 2.0f) - 0.01f;

    switch (key)
    {
    case GLUT_KEY_UP:
        beta_ += cameraAngleStep;
        if (beta_ > betaLimit)
            beta_ = betaLimit;
        break;
    case GLUT_KEY_DOWN:
        beta_ -= cameraAngleStep;
        if (beta_ < -betaLimit)
            beta_ = -betaLimit;
        break;
    case GLUT_KEY_LEFT:
        alpha -= cameraAngleStep;
        break;
    case GLUT_KEY_RIGHT:
        alpha += cameraAngleStep;
        break;
    }
    glutPostRedisplay();
}

void drawAsteroid(int type)
{
    switch (type)
    {
    case 1:
        asteroid1.draw();
        break;
    case 2:
        asteroid2.draw();
        break;
    case 3:
        asteroid3.draw();
        break;
    default:
        asteroid1.draw();
        break;
    }
}

void drawCinturaAsteroides()
{
    const int numAsteroids = 400;
    const float minRadius = 40.5f;
    const float maxRadius = 43.5f;
    const float minScale = 0.1f;
    const float maxScale = 0.3f;
    const float beltHeight = 1.5f;
    const float PI = 3.14159265359f;

    // seed fixa para os asteroides não mudarem de sítio em cada frame
    srand(12345);

    for (int i = 0; i < numAsteroids; i++)
    {
        float r = minRadius + ((float)rand() / (float)RAND_MAX) * (maxRadius - minRadius);
        float alpha = ((float)rand() / (float)RAND_MAX) * 2.0f * PI;
        float scale = minScale + ((float)rand() / (float)RAND_MAX) * (maxScale - minScale);
        float y = (((float)rand() / (float)RAND_MAX) - 0.5f) * beltHeight;

        float x = r * sin(alpha);
        float z = r * cos(alpha);

        glPushMatrix();
        glTranslatef(x, y, z);
        glScalef(scale, scale, scale);
        drawAsteroid((i % 3) + 1);

        glPopMatrix();
    }
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glDisable(GL_CULL_FACE);
    // Câmara usando coordenadas esféricas para permitir rotação
    float camX = radius * cos(beta_) * sin(alpha);
    float camY = radius * sin(beta_);
    float camZ = radius * cos(beta_) * cos(alpha);

    gluLookAt(camX, camY, camZ,
              sceneConfig.lookAt[0], sceneConfig.lookAt[1], sceneConfig.lookAt[2],
              sceneConfig.up[0], sceneConfig.up[1], sceneConfig.up[2]);

    // Desenho dos eixos
    if (axes)
        drawAxes();

    // glTranslatef(posX, 0.0f, posZ);
    // glRotatef(angle, 0.0f, 1.0f, 0.0f);
    // glScalef(1.0f, scaleY, 1.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, mode);
    drawModels();
    drawCinturaAsteroides();

    glutSwapBuffers();
}

// ------------------------------------------------

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "Uso: ./engine config.xml" << endl;
        return 1;
    }

    if (!sceneConfig.loadXML(argv[1]))
    {
        cout << "Erro ao carregar XML!" << endl;
        return 1;
    }
    asteroid1.load("./output/asteroid1.3d");
    asteroid2.load("./output/asteroid2.3d");
    asteroid3.load("./output/asteroid3.3d");

    radius = sqrt(pow(sceneConfig.posCam[0], 2) + pow(sceneConfig.posCam[1], 2) + pow(sceneConfig.posCam[2], 2));
    beta_ = asin(sceneConfig.posCam[1] / radius);
    alpha = atan2(sceneConfig.posCam[0], sceneConfig.posCam[2]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

    glutInitWindowSize(sceneConfig.width, sceneConfig.height);
    glutCreateWindow("CG Engine - Fase 2");

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(processSpecialKeys);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glutMainLoop();
    return 0;
}