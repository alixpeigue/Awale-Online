#ifndef ROOM_H
#define ROOM_H

#include "game.h"

typedef struct {
    uint32_t id;
    Game game;
} Room;

void room_init(Room *room);
void room_add_player(Room *room, Player player);
PlayResult room_play(Room *room, int pos, int side);

#endif
