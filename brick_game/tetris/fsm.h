#ifndef FSM_H
#define FSM_H

typedef enum {
  Begin,
  Spawn,
  Moving,
  Shifting,
  Attaching,
  GameOver,
  Paused,
} State;

#endif
