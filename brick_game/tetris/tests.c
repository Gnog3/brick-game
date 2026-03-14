#include <check.h>

#include "backend.h"
#include "defines.h"
#include "figures.h"
#include "fsm.h"
#include "save.h"
#include "update_funcs.h"

static void init(InternalGameState *gs) {
  memset(gs, 0, sizeof(InternalGameState));
  gs->next_figure = -1;
  gs->speed = DEFAULT_SPEED;
  gs->level = 1;
}

START_TEST(spawn_figure_test) {
  InternalGameState gs;
  init(&gs);
  gs.next_figure = 0;  // stick
  spawnNextFigure(&gs);
  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(gs.current_figure[0][i], 1);
  }
}

START_TEST(rotateFigure_1_test) {
  InternalGameState gs;
  init(&gs);
  gs.next_figure = 0;  // stick
  spawnNextFigure(&gs);
  gs.state = Moving;
  gs.action = Action;
  updateCurrentStateInternal(&gs);
  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(gs.current_figure[i][0], 1);
  }
}

START_TEST(rotateFigure_2_test) {
  InternalGameState gs;
  init(&gs);
  gs.next_figure = 3;  // square
  spawnNextFigure(&gs);
  rotateFigure(gs.current_figure);
  rotateFigure(gs.current_figure);
  ck_assert_int_eq(gs.current_figure[0][0], 1);
  ck_assert_int_eq(gs.current_figure[0][1], 1);
  ck_assert_int_eq(gs.current_figure[1][0], 1);
  ck_assert_int_eq(gs.current_figure[1][1], 1);
}

START_TEST(one_step_test) {
  InternalGameState gs;
  init(&gs);
  gs.next_figure = 0;  // stick
  gs.state = Spawn;
  updateCurrentStateInternal(&gs);
  gs.last_move_time = -10000;
  updateCurrentStateInternal(&gs);
  updateCurrentStateInternal(&gs);
  ck_assert_int_eq(gs.current_figure_x, FIGURE_SPAWN_X);
  ck_assert_int_eq(gs.current_figure_y, 1);
}

START_TEST(clear_line_test) {
  for (int lines = 1; lines <= 4; lines++) {
    InternalGameState gs;
    init(&gs);
    for (int i = 0; i < PLAYING_FIELD_WIDTH; i++) {
      for (int j = PLAYING_FIELD_HEIGHT - 1; j >= PLAYING_FIELD_HEIGHT - lines;
           j--) {
        gs.playing_field[j][i] = 1;
      }
    }
    gs.state = Attaching;
    updateCurrentStateInternal(&gs);
    for (int i = 0; i < PLAYING_FIELD_WIDTH; i++) {
      for (int j = PLAYING_FIELD_HEIGHT - 1; j >= PLAYING_FIELD_HEIGHT - lines;
           j--) {
        ck_assert_int_eq(gs.playing_field[j][i], 0);
      }
    }
  }
}

START_TEST(attach_test) {
  InternalGameState gs;
  init(&gs);
  gs.next_figure = 0;  // stick
  spawnNextFigure(&gs);
  gs.current_figure_x = 0;
  gs.current_figure_y = PLAYING_FIELD_HEIGHT - 1;
  gs.state = Moving;
  gs.last_move_time = -10000;
  updateCurrentStateInternal(&gs);
  updateCurrentStateInternal(&gs);
  ck_assert_int_eq(gs.state, Attaching);
  updateCurrentStateInternal(&gs);
  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(gs.playing_field[PLAYING_FIELD_HEIGHT - 1][i], 1);
  }
}

START_TEST(start_game_test) {
  InternalGameState gs;
  init(&gs);
  gs.action = Start;
  updateCurrentStateInternal(&gs);
  ck_assert_int_eq(gs.state, Spawn);
  updateCurrentStateInternal(&gs);
  ck_assert_int_ne(gs.next_figure, -1);
}

START_TEST(game_over_test) {
  InternalGameState gs;
  init(&gs);
  for (int i = 0; i < PLAYING_FIELD_HEIGHT; i++) {
    for (int j = 0; j < PLAYING_FIELD_WIDTH; j++) {
      gs.playing_field[i][j] = 1;
    }
  }
  gs.state = Spawn;
  updateCurrentStateInternal(&gs);
  ck_assert_int_eq(gs.state, GameOver);
}

START_TEST(pause_test) {
  InternalGameState gs;
  init(&gs);
  gs.state = Moving;
  gs.action = Pause;
  updateCurrentStateInternal(&gs);
  ck_assert_int_eq(gs.state, Paused);
  gs.action = Pause;
  updateCurrentStateInternal(&gs);
  ck_assert_int_eq(gs.state, Moving);
}

START_TEST(save_test) {
  saveHighscore(500);
  int score = loadHighscore();
  ck_assert_int_eq(score, 500);
}

Suite *tetris_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Tetris");

  tc_core = tcase_create("Core");
  tcase_add_test(tc_core, spawn_figure_test);
  tcase_add_test(tc_core, rotateFigure_1_test);
  tcase_add_test(tc_core, rotateFigure_2_test);
  tcase_add_test(tc_core, one_step_test);
  tcase_add_test(tc_core, clear_line_test);
  tcase_add_test(tc_core, attach_test);
  tcase_add_test(tc_core, start_game_test);
  tcase_add_test(tc_core, game_over_test);
  tcase_add_test(tc_core, pause_test);
  tcase_add_test(tc_core, save_test);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = tetris_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? 0 : 1;
}
