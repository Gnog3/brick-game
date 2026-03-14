#include "figures.h"

#include <ncurses.h>

const int FIGURES[7][4][4] = {
    {{COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED},
     {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK},
     {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK},
     {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK}},
    {{COLOR_CYAN, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK},
     {COLOR_CYAN, COLOR_CYAN, COLOR_CYAN, COLOR_BLACK},
     {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK},
     {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK}},
    {{COLOR_BLACK, COLOR_BLACK, COLOR_YELLOW, COLOR_BLACK},
     {COLOR_YELLOW, COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK},
     {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK},
     {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK}},
    {{COLOR_WHITE, COLOR_WHITE, COLOR_BLACK, COLOR_BLACK},
     {COLOR_WHITE, COLOR_WHITE, COLOR_BLACK, COLOR_BLACK},
     {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK},
     {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK}},
    {{COLOR_BLACK, COLOR_GREEN, COLOR_GREEN, COLOR_BLACK},
     {COLOR_GREEN, COLOR_GREEN, COLOR_BLACK, COLOR_BLACK},
     {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK},
     {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK}},
    {{COLOR_BLACK, COLOR_BLUE, COLOR_BLACK, COLOR_BLACK},
     {COLOR_BLUE, COLOR_BLUE, COLOR_BLUE, COLOR_BLACK},
     {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK},
     {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK}},
    {{COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK, COLOR_BLACK},
     {COLOR_BLACK, COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK},
     {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK},
     {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK}}};

void truncate(int figure[4][4]) {
  int top_empty = 1;
  while (top_empty) {
    for (int i = 0; i < 4 && top_empty; i++) {
      if (figure[0][i] != COLOR_BLACK) {
        top_empty = 0;
      }
    }

    if (top_empty) {
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
          figure[i][j] = figure[i + 1][j];
        }
      }
      for (int i = 0; i < 4; i++) {
        figure[3][i] = COLOR_BLACK;
      }
    }
  }
  int left_empty = 1;
  while (left_empty) {
    for (int i = 0; i < 4 && left_empty; i++) {
      if (figure[i][0] != COLOR_BLACK) {
        left_empty = 0;
      }
    }

    if (left_empty) {
      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
          figure[i][j] = figure[i][j + 1];
        }
      }
      for (int i = 0; i < 4; i++) {
        figure[i][3] = COLOR_BLACK;
      }
    }
  }
}

void rotate_figure_no_truncate(int figure[4][4]) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (j > i) {
        int tmp = figure[i][j];
        figure[i][j] = figure[j][i];
        figure[j][i] = tmp;
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (j <= 1) {
        int tmp = figure[i][j];
        figure[i][j] = figure[i][3 - j];
        figure[i][3 - j] = tmp;
      }
    }
  }
}

void rotate_figure(int figure[4][4]) {
  rotate_figure_no_truncate(figure);
  truncate(figure);
}
