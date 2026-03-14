#include "update_funcs.h"

#include <string.h>

#include "backend.h"
#include "defines.h"
#include "figures.h"
#include "fsm.h"

void spawn_next_figure(game_state* gs) {
  memcpy(gs->current_figure, FIGURES[gs->next_figure], 4 * 4 * sizeof(int));
  gs->current_figure_x = FIGURE_SPAWN_X;
  gs->current_figure_y = FIGURE_SPAWN_Y;
}

bool is_figure_pos_valid(game_state* gs, int figure[4][4], int x, int y) {
  bool valid = true;
  int curr_y = y;
  for (int i = 0; i < 4 && valid; i++) {
    int curr_x = x;
    for (int j = 0; j < 4 && valid; j++) {
      if (figure[i][j] != COLOR_BLACK &&
          (curr_x < 0 || curr_x >= PLAYING_FIELD_WIDTH || curr_y < 0 ||
           curr_y >= PLAYING_FIELD_HEIGHT)) {
        valid = false;
      }
      if (figure[i][j] != COLOR_BLACK &&
          gs->playing_field[curr_y][curr_x] != COLOR_BLACK) {
        valid = false;
      }
      curr_x++;
    }
    curr_y++;
  }
  return valid;
}

void shift_current_figure(game_state* gs) {
  if (is_figure_pos_valid(gs, gs->current_figure, gs->current_figure_x,
                          gs->current_figure_y + 1)) {
    gs->current_figure_y++;
    gs->state = Moving;
    gs->last_move_time = get_current_time_ms();
  } else {
    gs->state = Attaching;
  }
}

void handle_movement(game_state* gs) {
  if (gs->action == Left) {
    if (is_figure_pos_valid(gs, gs->current_figure, gs->current_figure_x - 1,
                            gs->current_figure_y)) {
      gs->current_figure_x--;
    }
  } else if (gs->action == Right) {
    if (is_figure_pos_valid(gs, gs->current_figure, gs->current_figure_x + 1,
                            gs->current_figure_y)) {
      gs->current_figure_x++;
    }
  } else if (gs->action == Action) {
    int figure[4][4];
    memcpy(figure, gs->current_figure, 4 * 4 * sizeof(int));
    rotate_figure(figure);
    if (is_figure_pos_valid(gs, figure, gs->current_figure_x,
                            gs->current_figure_y)) {
      rotate_figure(gs->current_figure);
    }
  } else if (gs->action == Down) {
    while (gs->state != Attaching) {
      shift_current_figure(gs);
    }
  }
}

bool is_line_full(game_state* gs, int line) {
  bool full = line >= 0 && line < PLAYING_FIELD_HEIGHT;
  for (int i = 0; i < PLAYING_FIELD_WIDTH && full; i++) {
    full = gs->playing_field[line][i] != COLOR_BLACK;
  }
  return full;
}

void clear_line(game_state* gs, int line) {
  for (int i = line; i > 0; i--) {
    for (int j = 0; j < PLAYING_FIELD_WIDTH; j++) {
      gs->playing_field[i][j] = gs->playing_field[i - 1][j];
    }
  }
  for (int i = 0; i < PLAYING_FIELD_WIDTH; i++) {
    gs->playing_field[0][i] = COLOR_BLACK;
  }
}

void clear_full_lines(game_state* gs) {
  int first_full_line = -1;
  for (int i = 0; i < PLAYING_FIELD_HEIGHT && first_full_line == -1; i++) {
    if (is_line_full(gs, i)) {
      first_full_line = i;
    }
  }
  if (first_full_line != -1) {
    clear_line(gs, first_full_line);
    int number_of_full_lines = 1;
    for (int i = first_full_line + 1; i < PLAYING_FIELD_HEIGHT; i++) {
      if (is_line_full(gs, i)) {
        number_of_full_lines++;
        clear_line(gs, i);
      }
    }
    if (number_of_full_lines == 1) {
      gs->score += 100;
    } else if (number_of_full_lines == 2) {
      gs->score += 300;
    } else if (number_of_full_lines == 3) {
      gs->score += 700;
    } else if (number_of_full_lines == 4) {
      gs->score += 1500;
    }
  }
}

void attach(game_state* gs) {
  int curr_y = gs->current_figure_y;
  for (int i = 0; i < 4; i++) {
    int curr_x = gs->current_figure_x;
    for (int j = 0; j < 4; j++) {
      int color = gs->current_figure[i][j];
      if (color != COLOR_BLACK) {
        gs->playing_field[curr_y][curr_x] = color;
      }
      curr_x++;
    }
    curr_y++;
  }
  clear_full_lines(gs);
}

void update_score(game_state* gs) {
  if (gs->score > gs->highscore) {
    gs->highscore = gs->score;
  }
}

void update_level(game_state* gs) {
  gs->level = gs->score / 600 + 1;
  if (gs->level > 10) {
    gs->level = 10;
  }
  gs->speed = (float)DEFAULT_SPEED / (1.0 + 0.2 * (gs->level - 1));
}
