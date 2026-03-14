#ifndef UPDATE_FUNCS_H
#define UPDATE_FUNCS_H

#include "backend.h"

void updateScore(InternalGameState* s);
void updateLevel(InternalGameState* s);
void spawnNextFigure(InternalGameState* s);
bool isFigurePosValid(InternalGameState* s, int figure[4][4], int x, int y);
void shiftCurrentFigure(InternalGameState* s);
void handleMovement(InternalGameState* s);
void attach(InternalGameState* s);

#endif
