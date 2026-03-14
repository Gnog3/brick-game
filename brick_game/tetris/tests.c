#include <check.h>

#include "backend.h"
#include "defines.h"
#include "figures.h"
#include "fsm.h"
#include "save.h"
#include "update_funcs.h"

START_TEST(spawn_figure_test) {
  game_state gs;
  init_game_state(&gs);
  gs.next_figure = 0;  // stick
  spawn_next_figure(&gs);
  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(gs.current_figure[0][i], COLOR_RED);
  }
}

START_TEST(rotate_figure_1_test) {
  game_state gs;
  init_game_state(&gs);
  gs.next_figure = 0;  // stick
  spawn_next_figure(&gs);
  gs.state = Moving;
  gs.action = Action;
  updateCurrentState(&gs);
  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(gs.current_figure[i][0], COLOR_RED);
  }
}

START_TEST(rotate_figure_2_test) {
  game_state gs;
  init_game_state(&gs);
  gs.next_figure = 3;  // square
  spawn_next_figure(&gs);
  rotate_figure_no_truncate(gs.current_figure);
  rotate_figure(gs.current_figure);
  ck_assert_int_eq(gs.current_figure[0][0], COLOR_WHITE);
  ck_assert_int_eq(gs.current_figure[0][1], COLOR_WHITE);
  ck_assert_int_eq(gs.current_figure[1][0], COLOR_WHITE);
  ck_assert_int_eq(gs.current_figure[1][1], COLOR_WHITE);
}

START_TEST(one_step_test) {
  game_state gs;
  init_game_state(&gs);
  gs.next_figure = 0;  // stick
  gs.state = Spawn;
  updateCurrentState(&gs);
  gs.last_move_time = -10000;
  updateCurrentState(&gs);
  updateCurrentState(&gs);
  ck_assert_int_eq(gs.current_figure_x, FIGURE_SPAWN_X);
  ck_assert_int_eq(gs.current_figure_y, 1);
}

START_TEST(clear_line_test) {
  for (int lines = 1; lines <= 4; lines++) {
    game_state gs;
    init_game_state(&gs);
    for (int i = 0; i < PLAYING_FIELD_WIDTH; i++) {
      for (int j = PLAYING_FIELD_HEIGHT - 1; j >= PLAYING_FIELD_HEIGHT - lines;
           j--) {
        gs.playing_field[j][i] = COLOR_WHITE;
      }
    }
    gs.state = Attaching;
    updateCurrentState(&gs);
    for (int i = 0; i < PLAYING_FIELD_WIDTH; i++) {
      for (int j = PLAYING_FIELD_HEIGHT - 1; j >= PLAYING_FIELD_HEIGHT - lines;
           j--) {
        ck_assert_int_eq(gs.playing_field[j][i], COLOR_BLACK);
      }
    }
  }
}

START_TEST(attach_test) {
  game_state gs;
  init_game_state(&gs);
  gs.next_figure = 0;  // stick
  spawn_next_figure(&gs);
  gs.current_figure_x = 0;
  gs.current_figure_y = PLAYING_FIELD_HEIGHT - 1;
  gs.state = Moving;
  gs.last_move_time = -10000;
  updateCurrentState(&gs);
  updateCurrentState(&gs);
  ck_assert_int_eq(gs.state, Attaching);
  updateCurrentState(&gs);
  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(gs.playing_field[PLAYING_FIELD_HEIGHT - 1][i], COLOR_RED);
  }
}

START_TEST(user_input_test) {
  UserAction_t action;
  userInput(&action, '\n');
  ck_assert_int_eq(action, Begin);
  userInput(&action, KEY_LEFT);
  ck_assert_int_eq(action, Left);
  userInput(&action, KEY_RIGHT);
  ck_assert_int_eq(action, Right);
  userInput(&action, KEY_DOWN);
  ck_assert_int_eq(action, Down);
  userInput(&action, 'r');
  ck_assert_int_eq(action, action);
  userInput(&action, 'p');
  ck_assert_int_eq(action, Pause);
  userInput(&action, 'q');
  ck_assert_int_eq(action, Terminate);
}

START_TEST(start_game_test) {
  game_state gs;
  init_game_state(&gs);
  gs.action = Begin;
  updateCurrentState(&gs);
  ck_assert_int_eq(gs.state, Spawn);
  updateCurrentState(&gs);
  ck_assert_int_ne(gs.next_figure, -1);
}

START_TEST(game_over_test) {
  game_state gs;
  init_game_state(&gs);
  for (int i = 0; i < PLAYING_FIELD_HEIGHT; i++) {
    for (int j = 0; j < PLAYING_FIELD_WIDTH; j++) {
      gs.playing_field[i][j] = COLOR_WHITE;
    }
  }
  gs.state = Spawn;
  updateCurrentState(&gs);
  ck_assert_int_eq(gs.state, GameOver);
}

START_TEST(pause_test) {
  game_state gs;
  init_game_state(&gs);
  gs.state = Moving;
  gs.action = Pause;
  updateCurrentState(&gs);
  ck_assert_int_eq(gs.state, Paused);
  gs.action = Pause;
  updateCurrentState(&gs);
  ck_assert_int_eq(gs.state, Moving);
}

START_TEST(save_test) {
  save_highscore(500);
  int score = load_highscore();
  ck_assert_int_eq(score, 500);
}

Suite *tetris_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Tetris");

  tc_core = tcase_create("Core");
  tcase_add_test(tc_core, spawn_figure_test);
  tcase_add_test(tc_core, rotate_figure_1_test);
  tcase_add_test(tc_core, rotate_figure_2_test);
  tcase_add_test(tc_core, one_step_test);
  tcase_add_test(tc_core, clear_line_test);
  tcase_add_test(tc_core, attach_test);
  tcase_add_test(tc_core, user_input_test);
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
