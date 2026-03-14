#ifndef DEFINES_H
#define DEFINES_H

#define PLAYING_FIELD_WIDTH 10
#define PLAYING_FIELD_HEIGHT 20
#define INITIAL_SNAKE_LENGTH 4
#define WIN_LENGTH 200
#define HIGHSCORE_FILENAME "highscore_snake.txt"
#define DEFAULT_SPEED 500
enum class Direction { UP, DOWN, LEFT, RIGHT };

enum class GameState {
  SPAWNING,
  AWAITING_MOVE,
  PROCESSING_MOVE,
  GROWING,
  PAUSED,
  GAME_OVER
};

#endif
