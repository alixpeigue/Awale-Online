#include "room.h"
#include "game.h"
#include <stdlib.h>

void room_init(Room *room) {
    room->id = rand();
    game_init(&room->game);
}

void room_add_player(Room *room, Player player, uint8_t spectate) {
    game_add_player(&room->game, player, spectate);
}

PlayResult room_play(Room *room, int pos, int side) {
    return game_play(&room->game, pos, side);
}
