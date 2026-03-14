#include "save.h"

#include <stdio.h>

#include "defines.h"

void saveHighscore(int score) {
  FILE* file = fopen(SAVE_FILENAME, "w");
  if (file) {
    fprintf(file, "%d", score);
    fclose(file);
  }
}

int loadHighscore() {
  int score = 0;
  FILE* file = fopen(SAVE_FILENAME, "r");
  if (file) {
    fscanf(file, "%d", &score);
    fclose(file);
  }
  return score;
}
