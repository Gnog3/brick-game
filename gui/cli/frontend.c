#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>

#include "../../brick_game/lib.h"

#define DRAW_FIELD_X 1
#define DRAW_FIELD_Y 1

#define PLAYING_FIELD_X (DRAW_FIELD_X + 1)
#define PLAYING_FIELD_Y (DRAW_FIELD_Y + 1)

#define PLAYING_FIELD_COLOR COLOR_WHITE
#define PLAYING_FIELD_FRAME_CHAR '#'
#define FIGURES_CHAR '0'

#define START_TEXT_X (DRAW_FIELD_X + PLAYING_FIELD_WIDTH + 3)
#define START_TEXT_Y (PLAYING_FIELD_HEIGHT / 2 + DRAW_FIELD_Y - 1)

#define FIGURES_COUNT 7

#define NEXT_FIGURE_X START_TEXT_X
#define NEXT_FIGURE_Y (DRAW_FIELD_Y + 1)
#define NEXT_FIGURE_IMAGE_X NEXT_FIGURE_X
#define NEXT_FIGURE_IMAGE_Y (NEXT_FIGURE_Y + 1)

#define SCORE_X START_TEXT_X
#define SCORE_Y (NEXT_FIGURE_IMAGE_Y + 3)
#define SCORE_NUMBER_X SCORE_X
#define SCORE_NUMBER_Y (SCORE_Y + 1)

#define HIGHSCORE_X START_TEXT_X
#define HIGHSCORE_Y (SCORE_NUMBER_Y + 2)
#define HIGHSCORE_NUMBER_X HIGHSCORE_X
#define HIGHSCORE_NUMBER_Y (HIGHSCORE_Y + 1)

#define LEVEL_X START_TEXT_X
#define LEVEL_Y (HIGHSCORE_NUMBER_Y + 2)
#define LEVEL_NUMBER_X LEVEL_X
#define LEVEL_NUMBER_Y (LEVEL_Y + 1)

#define PAUSED_TEXT "PAUSED"
#define PAUSED_X \
  ((PLAYING_FIELD_X + PLAYING_FIELD_WIDTH / 2) - (strlen(PAUSED_TEXT) / 2))
#define PAUSED_Y (PLAYING_FIELD_Y + PLAYING_FIELD_HEIGHT / 2 - 1)

#define FPS 100

static void init_ncurses() {
  initscr();  // Инициализация ncurses
  use_default_colors();
  curs_set(0);            // Скрытие курсора
  noecho();               // Отключение отображения вводимых символов
  keypad(stdscr, TRUE);   // Включение обработки функциональных клавиш
  nodelay(stdscr, TRUE);  // Неблокирующий ввод
  start_color();
}

void print_matrix(int rows, int cols, int** matrix, int x, int y, int ch) {
  int curr_y = y;

  for (int i = 0; i < rows; i++) {
    int curr_x = x;
    for (int j = 0; j < cols; j++) {
      int value = matrix[i][j];
      if (value != 0) {
        mvaddch(curr_y, curr_x, ch);
      }
      curr_x++;
    }
    curr_y++;
  }
}

void print_frame(int x, int y, int rows, int cols, int ch) {
  int curr_y = y;
  for (int i = 0; i < rows; i++) {
    int curr_x = x;
    for (int j = 0; j < cols; j++) {
      if (curr_y == y || curr_y == (y + rows - 1) || curr_x == x ||
          curr_x == (x + cols - 1)) {
        mvaddch(curr_y, curr_x, ch);
      }
      curr_x++;
    }
    curr_y++;
  }
}

void print_playing_field(GameInfo_t* gs) {
  print_frame(DRAW_FIELD_X, DRAW_FIELD_Y, PLAYING_FIELD_HEIGHT + 2,
              PLAYING_FIELD_WIDTH + 2, PLAYING_FIELD_FRAME_CHAR);
  print_matrix(PLAYING_FIELD_HEIGHT, PLAYING_FIELD_WIDTH, gs->field,
               PLAYING_FIELD_X, PLAYING_FIELD_Y, FIGURES_CHAR);
}

void print_interface(GameInfo_t* gs) {
  if (gs->next) {
    mvprintw(NEXT_FIGURE_Y, NEXT_FIGURE_X, "NEXT FIGURE:");
    print_matrix(4, 4, gs->next, NEXT_FIGURE_IMAGE_X, NEXT_FIGURE_IMAGE_Y,
                 FIGURES_CHAR);
  }
  mvprintw(SCORE_Y, SCORE_X, "SCORE:");
  mvprintw(SCORE_NUMBER_Y, SCORE_NUMBER_X, "%d", gs->score);
  mvprintw(HIGHSCORE_Y, HIGHSCORE_X, "HIGHSCORE:");
  mvprintw(HIGHSCORE_NUMBER_Y, HIGHSCORE_NUMBER_X, "%d", gs->high_score);
  mvprintw(LEVEL_Y, LEVEL_X, "LEVEL:");
  mvprintw(LEVEL_NUMBER_Y, LEVEL_NUMBER_X, "%d", gs->level);
}

void print_pause() { mvprintw(PAUSED_Y, PAUSED_X, PAUSED_TEXT); }

void print_start() {
  mvprintw(START_TEXT_Y, START_TEXT_X, "PRESS ENTER");
  mvprintw(START_TEXT_Y + 1, START_TEXT_X, "TO START THE GAME");
}

void wait_for_start() {
  erase();
  print_start();
  refresh();

  while (getch() != '\n') {
  }
}

UserAction_t getUserAction(int key) {
  switch (key) {
    case '\n':
      return Start;
    case KEY_LEFT:
      return Left;
    case KEY_RIGHT:
      return Right;
    case KEY_DOWN:
      return Down;
    case KEY_UP:
      return Up;
    case 'r':
    case 'R':
      return Action;
    case 'p':
    case 'P':
      return Pause;
    case 'q':
    case 'Q':
      return Terminate;
    default:
      return 0;
  }
}

int main() {
  init_ncurses();
  srand(time(0));
  refresh();

  wait_for_start();

  int end_score = 0;

  while (true) {
    UserAction_t action = getUserAction(getch());
    userInput(action, true);
    GameInfo_t g = updateCurrentState();
    end_score = g.score;
    erase();
    if (!g.field) {
      break;
    }
    if (!g.pause) {
      print_playing_field(&g);
    } else {
      print_pause();
    }
    print_interface(&g);
    refresh();
  }
  endwin();
  printf("GAME OVER\nSCORE: %d\n", end_score);
}
