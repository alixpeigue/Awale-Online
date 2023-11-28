#include "game.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void game_init(Game *game) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        game->board[i] = INITIAL_SEED;
    }

    for (int i = 0; i < 2; ++i) {
        game->players[i].captured = 0;
        game->players[i].id = 0;
        game->players[i].name[0] = '\0';
    }
}

void game_add_player(Game *game, Player player) {
    int i = game->players[0].id == 0 ? 0 : 1;
    game->players[i].captured = 0;
    game->players[i].id = player.id;
    strcpy(game->players[i].name, player.name);
}

int game_board_can_capture(const uint8_t *board, int opposite_side,
                           int last_pos) {
    int capturable = 0;
    int offset = (BOARD_SIZE / 2) * opposite_side;

    if ((opposite_side == 1 && last_pos < BOARD_SIZE / 2) ||
        (opposite_side == 0 && last_pos >= BOARD_SIZE / 2)) {
        return capturable;
    }

    for (int i = last_pos; i >= offset; --i) {
        if (board[i] != 0 && board[i] != 2 && board[i] != 3) {
            capturable = 1;
            break;
        }
    }

    return capturable;
}

GameState game_is_ended(const Game *game) {
    if (game->players[0].captured > BOARD_SIZE * 2) {
        return P1WON;
    } else if (game->players[1].captured > BOARD_SIZE * 2) {
        return P2WON;
    } else if (game->players[0].captured == BOARD_SIZE * 2 &&
               game->players[1].captured == BOARD_SIZE * 2) {
        return DRAW;
    }

    return NOT_ENDED;
}

PlayResult game_play(Game *game, int pos, int side) {
    if ((pos < 0 || pos >= BOARD_SIZE) ||
        ((side == 0 && pos >= BOARD_SIZE / 2) ||
         (side == 1 && pos < BOARD_SIZE / 2))) {
        return OUT_OF_BOUNDS;
    } else if (game->board[pos] <= 0) {
        return EMPTY_HOUSE;
    }

    int seeds_to_sow = game->board[pos];
    game->board[pos] = 0;

    int i;
    for (i = pos + 1; seeds_to_sow > 0; ++i) {
        if ((i % BOARD_SIZE) != pos) {
            ++game->board[i % BOARD_SIZE];
            --seeds_to_sow;
        }
    }

    pos = (i - 1) % BOARD_SIZE;

    if (pos < 0) {
        pos += BOARD_SIZE;
    }

    if (game_board_can_capture(game->board, 1 - side, pos)) {
        int offset = (BOARD_SIZE / 2) * (1 - side);

        for (int i = pos;
             i >= offset && (game->board[i] == 2 || game->board[i] == 3); --i) {
            game->players[side].captured += game->board[i];
            game->board[i] = 0;
        }
    }

    return VALID_PLAY;
}

void game_board_print(const int8_t *board) {
    for (int i = BOARD_SIZE - 1; i >= BOARD_SIZE / 2; --i) {
        printf("%d ", board[i]);
    }

    puts("");

    for (int i = 0; i < BOARD_SIZE / 2; ++i) {
        printf("%d ", board[i]);
    }

    puts("");
}
