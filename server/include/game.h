#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#define BOARD_SIZE 12
#define INITIAL_SEED 4

typedef enum { NOT_ENDED, P1WON, P2WON, DRAW } GameState;

typedef struct {
    uint32_t id;
    char *name;
    int8_t captured;
} Player;

typedef struct {
    int8_t board[BOARD_SIZE];
    Player players[2];
} Game;

void game_init(Game *game);
int game_turn(Game *game, int pos, int side);
int board_can_capture(const int8_t *board, int opposite_side, int last_pos);
GameState game_is_ended(const Game *game);
void board_print(const int8_t *board);
#endif
