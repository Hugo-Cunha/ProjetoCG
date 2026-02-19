#include "Group.h"

Group::~Group() {
    for (auto t : transforms) delete t;
    for (auto c : children) delete c;
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