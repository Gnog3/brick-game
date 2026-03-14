#ifndef BACKEND_H
#define BACKEND_H

#include "../lib.h"

typedef struct {
  int state;
  UserAction_t action;

  int playing_field[PLAYING_FIELD_HEIGHT][PLAYING_FIELD_WIDTH];

  int current_figure[4][4];
  int current_figure_x;
  int current_figure_y;

  int next_figure;

  long last_move_time;
  long speed;

  int score;
  int highscore;
  int level;
} InternalGameState;

void userInputInternal(InternalGameState* s, UserAction_t action, bool hold);
void updateCurrentStateInternal(InternalGameState* s);

#endif
