#include "update_funcs.h"

#include <string.h>

#include "backend.h"
#include "defines.h"
#include "figures.h"
#include "fsm.h"
#include "save.h"

void updateScore(InternalGameState* s) {
  if (s->score > s->highscore) {
    s->highscore = s->score;
  }
}

void updateLevel(InternalGameState* s) {
  s->level = s->score / 600 + 1;
  if (s->level > 10) {
    s->level = 10;
  }
  s->speed = (float)DEFAULT_SPEED / (1.0 + 0.2 * (s->level - 1));
}

void spawnNextFigure(InternalGameState* s) {
  memcpy(s->current_figure, FIGURES[s->next_figure], 4 * 4 * sizeof(int));
  s->current_figure_x = FIGURE_SPAWN_X;
  s->current_figure_y = FIGURE_SPAWN_Y;
}

bool isFigurePosValid(InternalGameState* s, int figure[4][4], int x, int y) {
  bool valid = true;
  int curr_y = y;
  for (int i = 0; i < 4 && valid; i++) {
    int curr_x = x;
    for (int j = 0; j < 4 && valid; j++) {
      if (figure[i][j] != 0 && (curr_x < 0 || curr_x >= PLAYING_FIELD_WIDTH ||
                                curr_y < 0 || curr_y >= PLAYING_FIELD_HEIGHT)) {
        valid = false;
      }
      if (figure[i][j] != 0 && s->playing_field[curr_y][curr_x] != 0) {
        valid = false;
      }
      curr_x++;
    }
    curr_y++;
  }
  return valid;
}

void shiftCurrentFigure(InternalGameState* s) {
  if (isFigurePosValid(s, s->current_figure, s->current_figure_x,
                       s->current_figure_y + 1)) {
    s->current_figure_y++;
    s->state = Moving;
  } else {
    s->state = Attaching;
  }
}

void handleMovement(InternalGameState* s) {
  if (s->action == Left) {
    if (isFigurePosValid(s, s->current_figure, s->current_figure_x - 1,
                         s->current_figure_y)) {
      s->current_figure_x--;
    }
  } else if (s->action == Right) {
    if (isFigurePosValid(s, s->current_figure, s->current_figure_x + 1,
                         s->current_figure_y)) {
      s->current_figure_x++;
    }
  } else if (s->action == Action) {
    int figure[4][4];
    memcpy(figure, s->current_figure, 4 * 4 * sizeof(int));
    rotateFigure(figure);
    if (isFigurePosValid(s, figure, s->current_figure_x, s->current_figure_y)) {
      rotateFigure(s->current_figure);
    }
  } else if (s->action == Down) {
    while (s->state != Attaching) {
      shiftCurrentFigure(s);
    }
  }
}

static bool isLineFull(InternalGameState* s, int line) {
  bool full = line >= 0 && line < PLAYING_FIELD_HEIGHT;
  for (int i = 0; i < PLAYING_FIELD_WIDTH && full; i++) {
    full = s->playing_field[line][i] != 0;
  }
  return full;
}

static void clearLine(InternalGameState* s, int line) {
  for (int i = line; i > 0; i--) {
    for (int j = 0; j < PLAYING_FIELD_WIDTH; j++) {
      s->playing_field[i][j] = s->playing_field[i - 1][j];
    }
  }
  for (int i = 0; i < PLAYING_FIELD_WIDTH; i++) {
    s->playing_field[0][i] = 0;
  }
}

static void clearFullLines(InternalGameState* s) {
  int first_full_line = -1;
  for (int i = 0; i < PLAYING_FIELD_HEIGHT && first_full_line == -1; i++) {
    if (isLineFull(s, i)) {
      first_full_line = i;
    }
  }
  if (first_full_line != -1) {
    clearLine(s, first_full_line);
    int number_of_full_lines = 1;
    for (int i = first_full_line + 1; i < PLAYING_FIELD_HEIGHT; i++) {
      if (isLineFull(s, i)) {
        number_of_full_lines++;
        clearLine(s, i);
      }
    }
    if (number_of_full_lines == 1) {
      s->score += 100;
    } else if (number_of_full_lines == 2) {
      s->score += 300;
    } else if (number_of_full_lines == 3) {
      s->score += 700;
    } else if (number_of_full_lines == 4) {
      s->score += 1500;
    }
    if (s->score > s->highscore) {
      s->highscore = s->score;
      saveHighscore(s->score);
    }
  }
}

void attach(InternalGameState* s) {
  int curr_y = s->current_figure_y;
  for (int i = 0; i < 4; i++) {
    int curr_x = s->current_figure_x;
    for (int j = 0; j < 4; j++) {
      int color = s->current_figure[i][j];
      if (color != 0) {
        s->playing_field[curr_y][curr_x] = 1;
      }
      curr_x++;
    }
    curr_y++;
  }
  clearFullLines(s);
}
