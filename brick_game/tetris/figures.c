#include "figures.h"

const int FIGURES[7][4][4] = {
    {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};

static void truncate(int figure[4][4]) {
  int top_empty = 1;
  while (top_empty) {
    for (int i = 0; i < 4 && top_empty; i++) {
      if (figure[0][i] != 0) {
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
        figure[3][i] = 0;
      }
    }
  }
  int left_empty = 1;
  while (left_empty) {
    for (int i = 0; i < 4 && left_empty; i++) {
      if (figure[i][0] != 0) {
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
        figure[i][3] = 0;
      }
    }
  }
}

static void rotateFigureNoTruncate(int figure[4][4]) {
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

void rotateFigure(int figure[4][4]) {
  rotateFigureNoTruncate(figure);
  truncate(figure);
}