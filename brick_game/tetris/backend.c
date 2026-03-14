#include "backend.h"

#include <stdlib.h>
#include <sys/time.h>

#include "../lib.h"
#include "defines.h"
#include "fsm.h"
#include "update_funcs.h"

static unsigned long get_current_time_ms() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

void userInputInternal(InternalGameState* s, UserAction_t action, bool hold) {
  if (hold) {
    s->action = action;
  }
}

void updateCurrentStateInternal(InternalGameState* s) {
  updateScore(s);
  updateLevel(s);
  if (s->state == Begin) {
    if (s->action == Start) {
      s->state = Spawn;
    }
  } else if (s->state == Spawn) {
    if (s->next_figure == -1) {
      s->next_figure = rand() % FIGURES_COUNT;
    }
    spawnNextFigure(s);
    s->next_figure = rand() % FIGURES_COUNT;
    s->state = Moving;
    s->last_move_time = get_current_time_ms();
    if (!isFigurePosValid(s, s->current_figure, s->current_figure_x,
                          s->current_figure_y)) {
      s->state = GameOver;
    }
  } else if (s->state == Moving) {
    handleMovement(s);
    long current_time = get_current_time_ms();
    if (current_time - s->last_move_time > s->speed) {
      s->state = Shifting;
    }
    if (s->action == Pause) {
      s->state = Paused;
    } else if (s->action == Terminate) {
      s->state = GameOver;
    }
  } else if (s->state == Shifting) {
    shiftCurrentFigure(s);
    s->last_move_time = get_current_time_ms();
  } else if (s->state == Attaching) {
    attach(s);
    s->state = Spawn;
  } else if (s->state == Paused) {
    if (s->action == Pause) {
      s->state = Moving;
    } else if (s->action == Terminate) {
      s->state = GameOver;
    }
  }
  s->action = -1;
}
