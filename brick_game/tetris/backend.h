#ifndef BACKEND_H
#define BACKEND_H

#include <ncurses.h>

#include "defines.h"

typedef enum {
  Begin = 1,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

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
} game_state;

unsigned long get_current_time_ms();
void init_game_state(game_state* result);
void userInput(UserAction_t* action, int key);
void updateCurrentState(game_state* gs);

#endif
