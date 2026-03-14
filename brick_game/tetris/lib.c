#include "../lib.h"

#include <stddef.h>
#include <string.h>

#include "backend.h"
#include "defines.h"
#include "figures.h"
#include "fsm.h"
#include "save.h"

static InternalGameState internalGameState = {
    .next_figure = -1,
    .speed = DEFAULT_SPEED,
    .level = 1,
};
static bool initialized;
static int playing_field[PLAYING_FIELD_HEIGHT][PLAYING_FIELD_WIDTH];
static int* playing_field_ptr[PLAYING_FIELD_HEIGHT];
static int* next_field_ptr[4];

static void init() {
  if (!initialized) {
    internalGameState.highscore = loadHighscore();
    for (int i = 0; i < PLAYING_FIELD_HEIGHT; i++) {
      playing_field_ptr[i] = &playing_field[i][0];
    }
    initialized = true;
  }
}

static GameInfo_t getGameInfo() {
  GameInfo_t info;
  if (internalGameState.state != GameOver) {
    info.field = playing_field_ptr;
  } else {
    info.field = NULL;
  }
  memcpy(&playing_field[0][0], internalGameState.playing_field,
         sizeof(playing_field));
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (internalGameState.current_figure[i][j]) {
        playing_field[internalGameState.current_figure_y + i]
                     [internalGameState.current_figure_x + j] = 1;
      }
    }
  }
  if (internalGameState.next_figure >= 0) {
    for (int i = 0; i < 4; i++) {
      next_field_ptr[i] = (int*)FIGURES[internalGameState.next_figure][i];
    }
    info.next = next_field_ptr;
  } else {
    info.next = NULL;
  }
  info.score = internalGameState.score;
  info.high_score = internalGameState.highscore;
  info.level = internalGameState.level;
  info.speed = internalGameState.speed;
  info.pause = internalGameState.state == Paused;
  return info;
}

void userInput(UserAction_t action, bool hold) {
  init();
  userInputInternal(&internalGameState, action, hold);
}

GameInfo_t updateCurrentState() {
  init();
  updateCurrentStateInternal(&internalGameState);
  return getGameInfo();
}