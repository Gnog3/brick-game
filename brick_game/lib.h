#ifndef LIB_H
#define LIB_H

#include <stdbool.h>

#define PLAYING_FIELD_WIDTH 10
#define PLAYING_FIELD_HEIGHT 20

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int** field;
  int** next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();

#ifdef __cplusplus
}
#endif

#endif
