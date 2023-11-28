#include "protocol.h"
#include "client_server_protocol.h"
#include "server_client_protocol.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

size_t server_client_protocol_read(const uint8_t *buf, const Handlers *handlers,
                                   State *state) {
    ServerClientProtocol type = *buf;
    switch (type) {
    case CONNECTION_SUCCESSFUL:
        handlers->connection_successful(state);
        break;
    case CONNECTION_REFUSED: {
        const char *message = (const char *)&buf[1];
        handlers->connection_refused(state, message);
    } break;
    case ROOM_CREATION_SUCCESSFUL: {
        uint32_t room_id = *(uint32_t *)(buf + 1);
        handlers->room_creation_successful(state, room_id);
    } break;
    case ROOM_CREATION_REFUSED: {
        const char *message = (const char *)&buf[1];
        handlers->room_creation_refused(state, message);
    } break;
    case JOIN_ROOM_SUCCESSFUL: {
        uint8_t nb_users = buf[1];
        const char **users = calloc(nb_users, sizeof(char *));
        const char *str = (const char *)&buf[2];
        for (uint32_t i = 0; i < nb_users; ++i) {
            users[i] = str;
            str += strlen(str) + 1;
        }
        handlers->join_room_successful(state, nb_users, users);
        free(users);
    } break;
    case JOIN_ROOM_REFUSED: {
        const char *message = (const char *)&buf[1];
        handlers->join_room_refused(state, message);
    } break;
    case SPECTATE_ROOM_SUCCESSFUL: {
        handlers->spectate_room_successful(state);
    } break;
    case SPECTATE_ROOM_REFUSED: {
        const char *message = (const char *)&buf[1];
        handlers->spectate_room_refused(state, message);
    } break;
    case PLAYED: {
        uint8_t s_score = buf[1];
        uint8_t o_score = buf[2];
        uint8_t *board = (uint8_t *)&buf[3];
        handlers->played(state, s_score, o_score, board);
    } break;
    case GAME_START: {
        uint8_t pos = buf[1];
        handlers->game_start(state, pos);
    } break;
    case PLAYER_JOINED_ROOM: {
        const char *username = (const char *)&buf[1];
        handlers->player_joined_room(state, username);
    } break;
    case SPECTATOR_JOINED_ROOM: {
        const char *username = (const char *)&buf[1];
        handlers->spectator_joined_room(state, username);
    } break;
    case GAME_STOPPED: {
        uint8_t draw = buf[1];
        const char * winner = (char *)&buf[2];
        handlers->game_stopped(state, draw, winner);
    } break;
    case MESSAGE: {
        const char* username = (char *)&buf[1];
        const char* message = (char *)&buf[2+strlen(username)];
        handlers->message(state, username, message);
    }
    case INVALID_PLAY: {
        const char * message = (char *)&buf[1];
        handlers->invalid_play(state, message);
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
    buf[3] = pos;
    return size + 2;
}

size_t server_client_protocol_write_spectate_room(uint8_t *buf,
                                                  uint32_t room_id) {
    size_t size = server_client_protocol_write_join_room(buf, room_id);
    buf[2] = SPECTATE_ROOM;
    return size;
}

size_t server_client_protocol_write_send_message(uint8_t *buf, const char *message) {
    uint16_t size = 2 + strlen(message);
    *(uint16_t *)&buf[0] = size;
    buf[2] = SEND_MESSAGE;
    strcpy((char *)&buf[3], message);
    return size + 2;
}
