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
        uint8_t nb_spectators = buf[2];
        const char *users[2];
        const char *bios[2];
        const char *spectators[1024];
        const char *str = (const char *)&buf[3];
        for (uint32_t i = 0; i < nb_users; ++i) {
            users[i] = str;
            str += strlen(str) + 1;
            bios[i] = str;
            str += strlen(str) + 1;
        }
        for (uint32_t i = 0; i < nb_spectators; ++i) {
            spectators[i] = str;
            str += strlen(str) + 1;
        }
        handlers->join_room_successful(state, nb_users, nb_spectators, users,
                                       bios, spectators);
    } break;
    case JOIN_ROOM_REFUSED: {
        const char *message = (const char *)&buf[1];
        handlers->join_room_refused(state, message);
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
        const char *winner = (char *)&buf[2];
        handlers->game_stopped(state, draw, winner);
    } break;
    case MESSAGE: {
        const char *username = (char *)&buf[1];
        const char *message = (char *)&buf[2 + strlen(username)];
        handlers->message(state, username, message);
    } break;
    case INVALID_PLAY: {
        const char *message = (char *)&buf[1];
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

size_t server_client_protocol_write_join_room(uint8_t *buf, uint32_t room_id,
                                              uint8_t spectate) {
    uint16_t size = sizeof(room_id) + sizeof(spectate) + 1;
    *(uint16_t *)&buf[0] = size;
    buf[2] = JOIN_ROOM;
    *(uint32_t *)&buf[3] = room_id;
    buf[7] = spectate;
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

size_t server_client_protocol_write_send_message(uint8_t *buf,
                                                 const char *message) {
    uint16_t size = 2 + strlen(message);
    *(uint16_t *)&buf[0] = size;
    buf[2] = SEND_MESSAGE;
    strcpy((char *)&buf[3], message);
    return size + 2;
}

size_t server_client_protocol_write_set_biography(uint8_t *buf,
                                                  const char *bio) {
    uint16_t size = 2 + strlen(bio);
    *(uint16_t *)&buf[0] = size;
    buf[2] = SET_BIOGRAPHY;
    strcpy((char *)&buf[3], bio);
    return size + 2;
}
