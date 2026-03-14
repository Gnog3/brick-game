#ifndef UPDATE_FUNCS_H
#define UPDATE_FUNCS_H

#include "backend.h"

void spawn_next_figure(game_state* gs);
bool is_figure_pos_valid(game_state* gs, int figure[4][4], int x, int y);
void shift_current_figure(game_state* gs);
void handle_movement(game_state* gs);
bool is_line_full(game_state* gs, int line);
void clear_line(game_state* gs, int line);
void clear_full_lines(game_state* gs);
void attach(game_state* gs);
void update_score(game_state* gs);
void update_level(game_state* gs);

#endif
