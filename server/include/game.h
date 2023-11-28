#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#define BOARD_SIZE 12
#define INITIAL_SEED 4

typedef enum { NOT_ENDED, P1WON, P2WON, DRAW } GameState;

typedef enum { VALID_PLAY, OUT_OF_BOUNDS, EMPTY_HOUSE } PlayResult;

typedef struct {
    uint32_t id;
    char name[1024];
    int8_t captured;
} Player;

typedef struct {
    uint8_t board[BOARD_SIZE];
    Player players[2];
} Game;

void game_init(Game *game);
void game_add_player(Game *game, Player player);
PlayResult game_play(Game *game, int pos, int side);
int game_board_can_capture(const uint8_t *board, int opposite_side, int last_pos);
GameState game_is_ended(const Game *game);
void game_board_print(const int8_t *board);
#endif
