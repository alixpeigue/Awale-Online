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
    printf("Please enter your next move?\n");
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
    printf("What room do you want to join ?\n");
}

void action_spectate_room() {
    printf("What room do you mant to spectate ?\n");
}
