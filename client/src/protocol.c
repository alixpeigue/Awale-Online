#include "protocol.h"
#include "client_server_protocol.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

size_t server_client_protocol_read(const uint8_t *buf,
                                   const Handlers *handlers) {
    ServerClientProtocol type = *buf;
    switch (type) {
    case CONNECTION_SUCCESSFUL:
        handlers->connection_successful();
        break;
    case CONNECTION_REFUSED: {
        const char *message = (const char *)&buf[1];
        handlers->connection_refused(message);
    } break;
    case ROOM_CREATION_SUCCESSFUL: {
        uint32_t room_id = *(uint32_t *)(buf + 1);
        handlers->room_creation_successful(room_id);
    } break;
    case ROOM_CREATION_REFUSED: {
        const char *message = (const char *)&buf[1];
        handlers->room_creation_refused(message);
    } break;
    case JOIN_ROOM_SUCCESSFUL: {
        uint8_t nb_users = buf[1];
        const char **users = calloc(nb_users, sizeof(char *));
        const char *str = (const char *)&buf[1];
        for (uint32_t i = 0; i < nb_users; ++i) {
            users[i] = str;
            str += strlen(str) + 1;
        }
        handlers->join_room_successful(users, nb_users);
        free(users);
    } break;
    case JOIN_ROOM_REFUSED: {
        const char *message = (const char *)&buf[1];
        handlers->join_room_refused(message);
    } break;
    case PLAYED: {
        uint8_t played = buf[1];
        handlers->played(played);
    } break;
    case GAME_START: {
        uint8_t pos = buf[1];
        handlers->game_start(pos);
    } break;
    case PLAYER_JOINED_ROOM: {
        const char *username = (const char *)&buf[1];
        handlers->player_joined_room(username);
    } break;
    case SPECTATOR_JOINED_ROOM: {
        const char *username = (const char *)&buf[1];
        handlers->spectator_joined_room(username);
    } break;
    case GAME_STOPPED: {
        uint8_t winner = buf[1];
        handlers->game_stopped(winner);
    } break;
    }
    return 1;
}

size_t server_client_protocol_write_connect(uint8_t *buf, const char *name) {
    uint16_t size = strlen(name) + 2;
    *(uint16_t *)&buf[0] = size;
    buf[2] = CONNECT;
    strcpy((char *)&buf[3], name);
    return size + 2;
}

size_t server_client_protocol_write_create_room(uint8_t *buf) {
    uint16_t size = 1;
    *(uint16_t *)&buf[0] = size;
    buf[2] = CREATE_ROOM;
    return size + 2;
}

size_t server_client_protocol_write_join_room(uint8_t *buf, uint32_t room_id) {
    uint16_t size = sizeof(room_id) + 1;
    *(uint16_t *)&buf[0] = size;
    buf[2] = JOIN_ROOM;
    *(uint32_t *)&buf[3] = room_id;
    return size + 2;
}

size_t server_client_protocol_write_leave_room(uint8_t *buf) {
    uint16_t size = 1;
    *(uint16_t *)&buf[0] = size;
    buf[2] = LEAVE_ROOM;
    return size + 2;
}

size_t server_client_protocol_write_play(uint8_t *buf, uint8_t pos) {
    uint16_t size = sizeof(pos) + 1;
    *(uint16_t *)&buf[0] = size;
    buf[2] = PLAY;
    return size + 2;
}

size_t server_client_protocol_write_spectate_room(uint8_t *buf,
                                                  uint32_t room_id) {
    size_t size = server_client_protocol_write_join_room(buf, room_id);
    buf[2] = SPECTATE_ROOM;
    return size;
}
