#ifndef FSM_H
#define FSM_H

#include "backend.h"

typedef enum {
  Start = 0,
  Spawn,
  Moving,
  Shifting,
  Attaching,
  GameOver,
  Paused,
} State;

#endif
