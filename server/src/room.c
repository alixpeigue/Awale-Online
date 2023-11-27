#include "room.h"
#include "game.h"
#include <stdlib.h>

void room_init(Room *room) {
    room->id = rand();
    game_init(&room->game);
}

void room_add_player(Room *room, Player player) {
    game_add_player(&room->game, player);
}
