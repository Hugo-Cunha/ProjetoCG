#include "Group.h"

Group::~Group() {
    for (auto t : transforms) delete t;
    for (auto c : children) delete c;
}

Translate::Translate(float t, bool a, std::vector<Point> points) {
    this->time = t;
    this->align = a;
    this->p = points;
}

void Rotate::apply() {
    if (isAnimated && time > 0) {
        float elapsed = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // Tempo em segundos 
        float currentAngle = (elapsed * 360.0f) / time; // 360 graus no tempo especificado [cite: 126]
        glRotatef(currentAngle, x, y, z);
    } else {
        glRotatef(angle, x, y, z);
    }
}

void Translate::apply() {
    if (time > 0 && p.size() >= 4) { // Requer mínimo de 4 pontos [cite: 135]
        float pos[3], deriv[3];
        float elapsed = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
        float gt = fmod(elapsed, time) / time; // t entre 0 e 1 [cite: 122]

        getGlobalCatmullRomPoint(gt, pos, deriv);
        glTranslatef(pos[0], pos[1], pos[2]);

        if (align) applyAlignment(deriv); // Alinhamento com a curva 
    } else if (!p.empty()) {
        glTranslatef(p[0].x, p[0].y, p[0].z);
    }
}

void Group::draw() {
    // 1. Guarda a matriz atual
    glPushMatrix(); 
    // 2. Aplica as transformações deste grupo na ordem correta
    for (auto t : transforms) {
        t->apply();
    }
    // 3. Desenha os modelos deste grupo
    for (auto& fig : models) {
        fig.draw();
    }
    // 4. Desenha os filhos
    for (auto child : children) {
        child->draw();
    }
    // 5. Restaura a matriz
    glPopMatrix(); 
}

void Translate::getGlobalCatmullRomPoint(float gt, float *pos, float *deriv) {
    int size = p.size(); // Mínimo de 4 pontos
    float t = gt * size; 
    int index = floor(t);
    t = t - index;

    int i0 = (index + size - 1) % size;
    int i1 = (index) % size;
    int i2 = (index + 1) % size;
    int i3 = (index + 2) % size;

    // Calculamos para cada eixo individualmente usando x, y, z
    getCatmullRomPoint(t, p[i0], p[i1], p[i2], p[i3], pos, deriv);
}

void Translate::getCatmullRomPoint(float t, Point p0, Point p1, Point p2, Point p3, float *pos, float *deriv) {
    float m[4][4] = { {-0.5f,  1.5f, -1.5f,  0.5f},
                      { 1.0f, -2.5f,  2.0f, -0.5f},
                      {-0.5f,  0.0f,  0.5f,  0.0f},
                      { 0.0f,  1.0f,  0.0f,  0.0f} };
    float px[4] = {p0.x, p1.x, p2.x, p3.x};
    float py[4] = {p0.y, p1.y, p2.y, p3.y};
    float pz[4] = {p0.z, p1.z, p2.z, p3.z};

    float ax[4], ay[4], az[4];

    for (int i = 0; i < 4; i++) {
        ax[i] = ay[i] = az[i] = 0.0f;
        for (int j = 0; j < 4; j++) {
            ax[i] += m[i][j] * px[j];
            ay[i] += m[i][j] * py[j];
            az[i] += m[i][j] * pz[j];
        }
    }

    // Posição final (pos = at^3 + bt^2 + ct + d)
    pos[0] = ax[0]*t*t*t + ax[1]*t*t + ax[2]*t + ax[3];
    pos[1] = ay[0]*t*t*t + ay[1]*t*t + ay[2]*t + ay[3];
    pos[2] = az[0]*t*t*t + az[1]*t*t + az[2]*t + az[3];

    // Derivada/Tangente para o alinhamento (deriv = 3at^2 + 2bt + c) 
    deriv[0] = 3*ax[0]*t*t + 2*ax[1]*t + ax[2];
    deriv[1] = 3*ay[0]*t*t + 2*ay[1]*t + ay[2];
    deriv[2] = 3*az[0]*t*t + 2*az[1]*t + az[2];
}

void Translate::applyAlignment(float *deriv) {
    float x[3] = {deriv[0], deriv[1], deriv[2]};
    float y[3] = {0, 1, 0}; // Vetor Up padrão
    float z[3];

    // Normalizar X (tangente)
    float res = sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
    x[0] /= res; x[1] /= res; x[2] /= res;

    // Z = X x Y (Produto vetorial)
    z[0] = x[1]*y[2] - x[2]*y[1];
    z[1] = x[2]*y[0] - x[0]*y[2];
    z[2] = x[0]*y[1] - x[1]*y[0];
    res = sqrt(z[0]*z[0] + z[1]*z[1] + z[2]*z[2]);
    z[0] /= res; z[1] /= res; z[2] /= res;

    // Y = Z x X (Recalcular Y para ser ortogonal)
    y[0] = z[1]*x[2] - z[2]*x[1];
    y[1] = z[2]*x[0] - z[0]*x[2];
    y[2] = z[0]*x[1] - z[1]*x[0];

    float m[16] = {
        x[0], x[1], x[2], 0.0f,
        y[0], y[1], y[2], 0.0f,
        z[0], z[1], z[2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    glMultMatrixf(m);
}