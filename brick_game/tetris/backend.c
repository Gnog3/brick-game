#include "backend.h"

#include <ncurses.h>
#include <stdlib.h>
#include <sys/time.h>

#include "defines.h"
#include "fsm.h"
#include "save.h"
#include "update_funcs.h"

unsigned long get_current_time_ms() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

void init_game_state(game_state* gs) {
  gs->state = Start;
  gs->action = 0;
  for (int i = 0; i < PLAYING_FIELD_HEIGHT; i++) {
    for (int j = 0; j < PLAYING_FIELD_WIDTH; j++) {
      gs->playing_field[i][j] = COLOR_BLACK;
    }
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      gs->current_figure[i][j] = COLOR_BLACK;
    }
  }
  gs->current_figure_x = 0;
  gs->current_figure_y = 0;
  gs->next_figure = -1;
  gs->last_move_time = 0;
  gs->speed = DEFAULT_SPEED;
  gs->score = 0;
  gs->highscore = load_highscore();
  gs->level = 1;
}

void userInput(UserAction_t* action, int key) {
  if (key == '\n') {
    *action = Begin;
  } else if (key == KEY_LEFT) {
    *action = Left;
  } else if (key == KEY_RIGHT) {
    *action = Right;
  } else if (key == KEY_DOWN) {
    *action = Down;
  } else if (key == 'r' || key == 'R') {
    *action = Action;
  } else if (key == 'p' || key == 'P') {
    *action = Pause;
  } else if (key == 'q' || key == 'Q') {
    *action = Terminate;
  }
}

void updateCurrentState(game_state* gs) {
  update_score(gs);
  update_level(gs);
  if (gs->state == Start) {
    if (gs->action == Begin) {
      gs->state = Spawn;
    }
  } else if (gs->state == Spawn) {
    if (gs->next_figure == -1) {
      gs->next_figure = rand() % FIGURES_COUNT;
    }
    spawn_next_figure(gs);
    gs->next_figure = rand() % FIGURES_COUNT;
    gs->state = Moving;
    gs->last_move_time = get_current_time_ms();
    if (!is_figure_pos_valid(gs, gs->current_figure, gs->current_figure_x,
                             gs->current_figure_y)) {
      gs->state = GameOver;
    }
  } else if (gs->state == Moving) {
    handle_movement(gs);
    long current_time = get_current_time_ms();
    if (current_time - gs->last_move_time > gs->speed) {
      gs->state = Shifting;
    }
    if (gs->action == Pause) {
      gs->state = Paused;
    } else if (gs->action == Terminate) {
      gs->state = GameOver;
    }
  } else if (gs->state == Shifting) {
    shift_current_figure(gs);
  } else if (gs->state == Attaching) {
    attach(gs);
    gs->state = Spawn;
  } else if (gs->state == GameOver) {
    save_highscore(gs->highscore);
  } else if (gs->state == Paused) {
    if (gs->action == Pause) {
      gs->state = Moving;
    } else if (gs->action == Terminate) {
      gs->state = GameOver;
    }
  }
}
