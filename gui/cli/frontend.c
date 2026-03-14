#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../brick_game/tetris/backend.h"
#include "../../brick_game/tetris/defines.h"
#include "../../brick_game/tetris/figures.h"
#include "../../brick_game/tetris/fsm.h"

void init_colors() {
  for (int i = 0; i <= COLOR_WHITE; i++) {
    init_pair(i, i, -1);
  }
}

void init_ncurses() {
  initscr();  // Инициализация ncurses
  use_default_colors();
  curs_set(0);  // Скрытие курсора
  noecho();  // Отключение отображения вводимых символов
  keypad(stdscr, TRUE);  // Включение обработки функциональных клавиш
  nodelay(stdscr, TRUE);  // Неблокирующий ввод
  start_color();
  init_colors();
}

void print_matrix(int rows, int cols, const int matrix[rows][cols], int x,
                  int y, int ch) {
  int curr_y = y;

  for (int i = 0; i < rows; i++) {
    int curr_x = x;
    for (int j = 0; j < cols; j++) {
      int value = matrix[i][j];
      if (value != 0) {
        attron(COLOR_PAIR(value));
        mvaddch(curr_y, curr_x, ch);
        attroff(COLOR_PAIR(value));
      }
      curr_x++;
    }
    curr_y++;
  }
}

void print_frame(int x, int y, int rows, int cols, int color, int ch) {
  attron(COLOR_PAIR(color));
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
  attroff(COLOR_PAIR(color));
}

void print_playing_field(game_state* gs) {
  print_frame(DRAW_FIELD_X, DRAW_FIELD_Y, PLAYING_FIELD_HEIGHT + 2,
              PLAYING_FIELD_WIDTH + 2, PLAYING_FIELD_COLOR,
              PLAYING_FIELD_FRAME_CHAR);
  print_matrix(PLAYING_FIELD_HEIGHT, PLAYING_FIELD_WIDTH, gs->playing_field,
               PLAYING_FIELD_X, PLAYING_FIELD_Y, FIGURES_CHAR);
}

void print_interface(game_state* gs) {
  if (gs->state == Start) {
    mvprintw(START_TEXT_Y, START_TEXT_X, "PRESS ENTER");
    mvprintw(START_TEXT_Y + 1, START_TEXT_X, "TO START THE GAME");
  } else {
    mvprintw(NEXT_FIGURE_Y, NEXT_FIGURE_X, "NEXT FIGURE:");
    print_matrix(4, 4, FIGURES[gs->next_figure], NEXT_FIGURE_IMAGE_X,
                 NEXT_FIGURE_IMAGE_Y, FIGURES_CHAR);
    mvprintw(SCORE_Y, SCORE_X, "SCORE:");
    mvprintw(SCORE_NUMBER_Y, SCORE_NUMBER_X, "%d", gs->score);
    mvprintw(HIGHSCORE_Y, HIGHSCORE_X, "HIGHSCORE:");
    mvprintw(HIGHSCORE_NUMBER_Y, HIGHSCORE_NUMBER_X, "%d", gs->highscore);
    mvprintw(LEVEL_Y, LEVEL_X, "LEVEL:");
    mvprintw(LEVEL_NUMBER_Y, LEVEL_NUMBER_X, "%d", gs->level);
  }
}

void print_current_figure(game_state* gs) {
  if (gs->state == Moving) {
    print_matrix(4, 4, gs->current_figure,
                 gs->current_figure_x + PLAYING_FIELD_X,
                 gs->current_figure_y + PLAYING_FIELD_Y, FIGURES_CHAR);
  }
}

void print_pause() { mvprintw(PAUSED_Y, PAUSED_X, PAUSED_TEXT); }

int main() {
  init_ncurses();
  srand(time(0));
  refresh();
  game_state gs;
  init_game_state(&gs);
  // print_playing_field(gs.playing_field);

  // int a[4][4];
  while (gs.state != GameOver) {
    gs.action = 0;
    userInput(&gs.action, getch());
    updateCurrentState(&gs);
    erase();
    if (gs.state != Paused) {
      print_playing_field(&gs);
      print_current_figure(&gs);
    } else {
      print_pause();
    }
    print_interface(&gs);
    refresh();
  }
  updateCurrentState(&gs);

  endwin();
  printf("GAME OVER\nSCORE: %d\n", gs.score);
}
