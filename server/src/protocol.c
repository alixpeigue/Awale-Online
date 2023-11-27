#include "protocol.h"
#include "client_server_protocol.h"
#include "server.h"
#include "server_client_protocol.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int nb_clients;
extern int current_client;
extern Client clients[MAX_CLIENTS];
extern char buffer[BUF_SIZE];
extern int payload_size;

size_t client_server_protocol_read(const uint8_t *buf,
                                   const Handlers *handlers) {
    ClientServerProtocol type = *buf;
    switch (type) {
    case CONNECT:
        fprintf(stderr, "switch case connect client_server_protocol_connect\n");
        handlers->connect((const char *)&buf[1]);
        break;
    case CREATE_ROOM: {
        handlers->create_room();
    } break;
    case JOIN_ROOM: {
        uint32_t room_id = *(uint32_t *)&buf[1];
        handlers->join_room(room_id);
    } break;
    case SPECTATE_ROOM: {
        uint32_t room_id = *(uint32_t *)&buf[1];
        handlers->spectate_room(room_id);
    } break;
    case PLAY: {
        uint8_t pos = *(uint8_t *)&buf[1];
        handlers->play(pos);
    } break;
    case LEAVE_ROOM: {
        handlers->leave_room();
    } break;
    }
    return 1;
}

void client_server_protocol_connect(const char *name) {
    int name_in_use = 0;

    fprintf(stderr, "client_server_protocol_connect\n");
    for (int i = 0; i < nb_clients; ++i) {
        if (strcmp(clients[i].name, name) == 0) {
            name_in_use = 1;
            break;
        }
    }

    if (name_in_use) {
        fprintf(stderr,
                "client_server_protocol_connect name (%s, length %lu) in use\n",
                name, strlen(name));
        payload_size = server_client_protocol_write_connection_refused(
            (uint8_t *)buffer, "Error: Name already in use.");
    } else {
        fprintf(
            stderr,
            "client_server_protocol_connect name (%s, length %lu) available\n",
            name, strlen(name));
        strcpy(clients[current_client].name, name);
        payload_size = server_client_protocol_write_connection_successful(
            (uint8_t *)buffer);
    }
}

void client_server_protocol_create_room(void) {}

void client_server_protocol_join_room(uint32_t room_id) {}

void client_server_protocol_spectate_room(uint32_t room_id) {}

void client_server_protocol_play(uint8_t play) {}

void client_server_protocol_leave_room(void) {}

size_t server_client_protocol_write_connection_successful(uint8_t *buf) {
    uint16_t size = 1;
    *(uint16_t *)&buf[0] = size;
    buf[2] = CONNECTION_SUCCESSFUL;
    return size + 2;
}

size_t
server_client_protocol_write_connection_refused(uint8_t *buf,
                                                const char *error_message) {
    uint16_t size = strlen(error_message) + 2;
    *(uint16_t *)&buf[0] = size;
    buf[2] = CONNECTION_REFUSED;
    strcpy((char *)&buf[3], error_message);
    return size + 2;
}

size_t server_client_protocol_write_room_creation_successful(uint8_t *buf) {
    uint32_t room_id = rand();
    uint16_t size = sizeof(room_id) + 1;
    *(uint16_t *)&buf[0] = size;
    buf[2] = ROOM_CREATION_SUCCESSFUL;
    *(uint32_t *)&buf[3] = room_id;
    return size + 2;
}

size_t
server_client_protocol_write_room_creation_refused(uint8_t *buf,
                                                   const char *error_message) {
    uint16_t size = strlen(error_message) + 2;
    *(uint16_t *)&buf[0] = size;
    buf[2] = ROOM_CREATION_REFUSED;
    strcpy((char *)&buf[3], error_message);
    return size + 2;
}

size_t server_client_protocol_write_join_room_successful(uint8_t *buf,
                                                         const char **users,
                                                         uint8_t nb_users) {
    uint16_t size = 1;

    for (int i = 0; i < nb_users; ++i) {
        strcpy((char *)&buf[size + 3], users[i]);

        size += strlen(users[i]) + 1;
    }

    *(uint16_t *)&buf[0] = size;
    buf[2] = JOIN_ROOM_SUCCESSFUL;
    return size + 2;
}

size_t
server_client_protocol_write_join_room_refused(uint8_t *buf,
                                               const char *error_message) {
    uint16_t size = strlen(error_message) + 2;
    *(uint16_t *)&buf[0] = size;
    buf[2] = JOIN_ROOM_REFUSED;
    strcpy((char *)&buf[3], error_message);
    return size + 2;
}

size_t server_client_protocol_write_played(uint8_t *buf, uint8_t pos) {
    uint16_t size = sizeof(pos) + 1;
    *(uint16_t *)&buf[0] = size;
    buf[2] = PLAYED;
    *(uint8_t *)&buf[3] = pos;
    return size + 2;
}

size_t server_client_protocol_write_game_start(uint8_t *buf, uint8_t side) {
    uint16_t size = sizeof(side) + 1;
    *(uint16_t *)&buf[0] = size;
    buf[2] = GAME_START;
    *(uint8_t *)&buf[3] = side;
    return size + 2;
}

size_t server_client_protocol_write_player_joined_room(uint8_t *buf,
                                                       const char *username) {
    uint16_t size = strlen(username) + 2;
    *(uint16_t *)&buf[0] = size;
    buf[2] = PLAYER_JOINED_ROOM;
    strcpy((char *)&buf[3], username);
    return size + 2;
}

size_t
server_client_protocol_write_spectator_joined_room(uint8_t *buf,
                                                   const char *username) {
    uint16_t size = strlen(username) + 2;
    *(uint16_t *)&buf[0] = size;
    buf[2] = SPECTATOR_JOINED_ROOM;
    strcpy((char *)&buf[3], username);
    return size + 2;
}

size_t server_client_protocol_write_game_stopped(uint8_t *buf, uint8_t winner) {
    uint16_t size = sizeof(winner) + 1;
    *(uint16_t *)&buf[0] = size;
    buf[2] = GAME_STOPPED;
    *(uint8_t *)&buf[3] = winner;
    return size + 2;
}

void handlers_init(Handlers *handlers) {
    handlers->connect = client_server_protocol_connect;
    handlers->create_room = client_server_protocol_create_room;
    handlers->join_room = client_server_protocol_join_room;
    handlers->leave_room = client_server_protocol_leave_room;
    handlers->play = client_server_protocol_play;
    handlers->spectate_room = client_server_protocol_spectate_room;
}
