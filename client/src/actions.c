#include "actions.h"
#include "client.h"
#include "protocol.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void action_connect() {
    printf("What is your name?\n");
}

void action_play() {
    printf("What do you want to play ?\n");
}

void action_choose() {
    printf("What do you want to do ?\n 1 - Create a room\n 2 - Join a room\n 3 "
           "- Spectate a room\n");
}

void action_create_room() {
    uint8_t buf[1024];
    size_t size = server_client_protocol_write_create_room(buf);
    write_server((char *)buf, size);
}

void action_join_room() {
    uint8_t buf[1024];
    printf("What room do you want to join ?\n> ");
    uint32_t room_id;
    scanf("%x", &room_id);
    size_t size = server_client_protocol_write_join_room(buf, room_id);
    write_server((char *)buf, size);
}

void action_spectate_room() {
    uint8_t buf[1024];
    printf("What room do you mant to spectate ?\n> ");
    uint32_t room_id;
    scanf("%x", &room_id);
    size_t size = server_client_protocol_write_spectate_room(buf, room_id);
    write_server((char *)buf, size);
}
