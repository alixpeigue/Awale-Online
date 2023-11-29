#ifndef ACTIONS_H
#define ACTIONS_H

#include <stdint.h>
void action_connect(void);
void action_choose(void);
void action_play(void);
void action_create_room(void);
void action_join_room(void);
void action_spectate_room(void);
void action_bio();
void action_show_board(uint8_t s_score, uint8_t o_score, uint8_t *board);

#endif
