#include "protocol.h"
#include "client_server_protocol.h"
#include "game.h"
#include "room.h"
#include "server.h"
#include "server_client_protocol.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern int current_client;
extern int nb_clients;
extern Client clients[MAX_CLIENTS];
extern int nb_rooms;
extern Room rooms[MAX_ROOMS];

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
        uint8_t spectate = buf[sizeof(room_id) + 1];
        handlers->join_room(room_id, spectate);
    } break;
    case PLAY: {
        uint8_t pos = *(uint8_t *)&buf[1];
        handlers->play(pos);
    } break;
    case LEAVE_ROOM: {
        handlers->leave_room();
    } break;
    case SEND_MESSAGE: {
        char *message = (char *)(buf + 1);
        handlers->send_message(message);
    } break;
    case SET_BIOGRAPHY: {
        char *biography = (char *)(buf + 1);
        handlers->set_biography(biography);
    } break;
    }
    return 1;
}

void handle_connect(const char *name) {
    uint8_t buffer[BUF_SIZE];
    int name_in_use = 0;
    size_t payload_size = 0;
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
            buffer, "Error: Name already in use.");
    } else {
        strcpy(clients[current_client].name, name);
        fprintf(
            stderr,
            "client_server_protocol_connect name (%s, length %lu) available\n",
            name, strlen(name));

        for (int i = 0; i < nb_rooms; ++i) {
            for (int j = 0; j < rooms[i].game.nb_players; ++j) {
                if (strcmp(name, rooms[i].game.players[j].name) == 0) {
                    rooms[i].game.players[j].id = clients[current_client].sock;
                    if(!rooms[i].game.is_started) {
                        clients[current_client].room_id = rooms[i].id;
                        const char *player_names[MAX_GAME_PLAYERS];
                        const char *player_bios[2];

                        for (int k = 0; k < rooms[i].game.nb_players; ++k) {
                            player_names[k] = rooms[i].game.players[k].name;
                            player_bios[k] =
                                clients[rooms[i].game.players[k].client_index].biography;
                            fprintf(stderr, "BIO : %s \n", player_bios[k]);
                        }

                        for (int k = 0; k < rooms[i].game.nb_spectators; ++k) {
                            player_names[rooms[i].game.nb_players + k] =
                                rooms[i].game.players[2 + k].name;
                        }
                        payload_size = server_client_protocol_write_join_room_successful(
                            buffer, player_names, rooms[i].game.nb_players,
                            rooms[i].game.nb_spectators, player_bios);
                        write_client(clients[current_client].sock, (char *)buffer,
                                     payload_size);
                    } else if (rooms[i].game.turn == j) {
                        clients[current_client].room_id = rooms[i].id;
                        payload_size = server_client_protocol_write_played(
                            buffer, j, &rooms[i].game);
                        write_client(clients[current_client].sock,
                                     (char *)buffer, payload_size);
                    } else {
                        clients[current_client].room_id = rooms[i].id;
                        payload_size =
                            server_client_protocol_write_game_start(buffer, 1);
                        write_client(clients[current_client].sock,
                                     (char *)buffer, payload_size);
                    }
                    return;
                }
            }
        }

        // user wasn't disconnected
        payload_size =
            server_client_protocol_write_connection_successful(buffer);
    }

    write_client(clients[current_client].sock, (char *)buffer, payload_size);
}

void handle_create_room(void) {
    uint8_t buffer[BUF_SIZE];
    room_init(&rooms[nb_rooms]);

    Player player;
    player.id = clients[current_client].sock;
    clients[current_client].room_id = rooms[nb_rooms].id;
    strcpy(player.name, clients[current_client].name);
    player.captured = 0;
    player.client_index = current_client;
    room_add_player(&rooms[nb_rooms], player, 0);
    size_t payload_size = server_client_protocol_write_room_creation_successful(
        buffer, rooms[nb_rooms].id);
    write_client(clients[current_client].sock, (char *)buffer, payload_size);

    ++nb_rooms;
}

void handle_join_room(uint32_t room_id, uint8_t spectate) {
    uint8_t buffer[BUF_SIZE];
    uint32_t payload_size = 0;
    int room_found = 0;
    int room_full = 0;

    int i;
    for (i = 0; i < nb_rooms; ++i) {
        if (rooms[i].id == room_id) {

            if ((!spectate && rooms[i].game.nb_players >= 2) ||
                rooms[i].game.nb_players >= MAX_GAME_PLAYERS) {
                room_full = 1;
                break;
            }

            room_found = 1;
            break;
        }
    }

    if (room_found) {
        Player player;
        player.id = clients[current_client].sock;
        strcpy(player.name, clients[current_client].name);
        player.captured = 0;
        player.client_index = current_client;
        room_add_player(&rooms[i], player, spectate);

        const char *player_names[MAX_GAME_PLAYERS];
        const char *player_bios[2];

        for (int j = 0; j < rooms[i].game.nb_players; ++j) {
            player_names[j] = rooms[i].game.players[j].name;
            player_bios[j] =
                clients[rooms[i].game.players[j].client_index].biography;
            fprintf(stderr, "BIO : %s \n", player_bios[j]);
        }

        for (int j = 0; j < rooms[i].game.nb_spectators; ++j) {
            player_names[rooms[i].game.nb_players + j] =
                rooms[i].game.players[2 + j].name;
        }

        clients[current_client].room_id = room_id;
        payload_size = server_client_protocol_write_join_room_successful(
            buffer, player_names, rooms[i].game.nb_players,
            rooms[i].game.nb_spectators, player_bios);
        write_client(clients[current_client].sock, (char *)buffer,
                     payload_size);

        if (rooms[i].game.nb_players >= 2 && !rooms[i].game.is_started) {
            for (int j = 0; j < rooms[i].game.nb_players; ++j) {
                payload_size =
                    server_client_protocol_write_game_start(buffer, j);
                write_client(rooms[i].game.players[j].id, (char *)buffer,
                             payload_size);
                rooms[i].game.is_started = 1;
            }
        }

    } else if (room_full) {
        payload_size = server_client_protocol_write_join_room_refused(
            buffer, "Error: The room is full. Try to spectate instead.");
        write_client(clients[current_client].sock, (char *)buffer,
                     payload_size);
    } else {
        payload_size = server_client_protocol_write_join_room_refused(
            buffer, "Error: Room not found.");
        write_client(clients[current_client].sock, (char *)buffer,
                     payload_size);
    }
}

void handle_play(uint8_t play) {
    for (int i = 0; i < nb_rooms; ++i) {
        if (clients[current_client].room_id == rooms[i].id) {
            int player_side =
                rooms[i].game.players[0].id == clients[current_client].sock ? 0
                                                                            : 1;
            PlayResult res = room_play(
                &rooms[i], player_side * BOARD_SIZE / 2 + play, player_side);
            uint8_t buffer[1024];
            size_t payload_size = 0;
            if (res == VALID_PLAY) {

                for (int j = 0; j < rooms[i].game.nb_players; ++j) {
                    payload_size = server_client_protocol_write_played(
                        buffer, j, &rooms[i].game);
                    write_client(rooms[i].game.players[j].id, (char *)buffer,
                                 payload_size);
                }

                for (int j = 0; j < rooms[i].game.nb_spectators; ++j) {
                    payload_size = server_client_protocol_write_played(
                        buffer, 0, &rooms[i].game);
                    write_client(rooms[i].game.players[2 + j].id,
                                 (char *)buffer, payload_size);
                }
            } else {
                if (res == OUT_OF_BOUNDS) {
                    payload_size = server_client_protocol_write_invalid_play(
                        buffer, "Error: Invalid play.");
                } else if (res == EMPTY_HOUSE) {
                    payload_size = server_client_protocol_write_invalid_play(
                        buffer, "Error: Empty house.");
                } else {
                    payload_size = server_client_protocol_write_invalid_play(
                        buffer, "Unknown error.");
                }

                write_client(rooms[i].game.players[player_side].id,
                             (char *)buffer, payload_size);
            }
            GameState gs = game_is_ended(&rooms[i].game);
            if (gs != NOT_ENDED) {
                uint8_t draw = gs == DRAW;
                const char *winner;
                if (!draw) {
                    winner = rooms[i].game.players[gs - 1].name;
                }
                payload_size = server_client_protocol_write_game_stopped(
                    buffer, gs == DRAW, winner);
                write_client(rooms[i].game.players[0].id, (char *)buffer,
                             payload_size);
                write_client(rooms[i].game.players[1].id, (char *)buffer,
                             payload_size);

                // Delete room from rooms list
                --nb_rooms;
                for (int k = i; k < nb_rooms; ++k) {
                    rooms[k] = rooms[k + 1];
                }
            }
            break;
        }
    }
}

void handle_leave_room(void) {
    uint32_t room_id = clients[current_client].room_id;
    clients[current_client].room_id = 0;

    int i;
    for (i = 0; i < nb_rooms; ++i) {
        if (rooms[i].id == room_id) {
            break;
        }
    }

    for (int j = 0; j < rooms[i].game.nb_players; ++j) {
        if (rooms[i].game.players[j].id == clients[current_client].sock) {
            // A player is leaving, close room
            uint8_t buffer[1024];
            size_t payload_size;
            payload_size =
                server_client_protocol_write_game_stopped(buffer, 1, "");
            for (int k = 0;
                 k < rooms[i].game.nb_players + rooms[i].game.nb_spectators;
                 ++k) {
                if (k != j) {
                    write_client(rooms[i].game.players[k].id, (char *)buffer,
                                 payload_size);
                }
            }

            // Delete room from rooms list
            --nb_rooms;
            for (int k = i; k < nb_rooms; ++k) {
                rooms[k] = rooms[k + 1];
            }

            return;
        }
    }

    // If not player, then spectator : delete from spectators list

    for (int j = 0; j < rooms[i].game.nb_spectators; ++j) {
        if (rooms[i].game.players[j + 2].id == clients[current_client].sock) {
            // remove spectator from game
            rooms[i].game.players[j + 2] =
                rooms[i].game.players[rooms[i].game.nb_spectators - 1];
            --rooms[i].game.nb_spectators;
            return;
        }
    }
}

void handle_send_message(const char *message) {
    uint32_t room_id = clients[current_client].room_id;

    int i;
    for (i = 0; i < nb_rooms; ++i) {
        if (rooms[i].id == room_id) {
            break;
        }
    }

    uint8_t buffer[1024];
    // TODO: handle spectate and nb_users_in_room
    size_t payload_size = server_client_protocol_write_send_message_to_room(
        buffer, clients[current_client].name, message);

    for (int j = 0; j < rooms[i].game.nb_players; ++j) {
        if (rooms[i].game.players[j].id != clients[current_client].sock) {
            write_client(rooms[i].game.players[j].id, (char *)buffer,
                         payload_size);
        }
    }

    for (int j = 2; j < rooms[i].game.nb_spectators + 2; ++j) {
        if (rooms[i].game.players[j].id != clients[current_client].sock) {
            write_client(rooms[i].game.players[j].id, (char *)buffer,
                         payload_size);
        }
    }
}

void handle_set_biography(const char *biography) {
    strcpy(clients[current_client].biography, biography);
}

void disconnect_user() {
    for (int i = 0; i < nb_rooms; ++i) {
        if (clients[current_client].room_id == rooms[i].id) {
            for (int j = 0; j < rooms[i].game.nb_players; ++j) {
                if (rooms[i].game.players[j].id ==
                    clients[current_client].sock) {
                    rooms[i].game.players[j].id = -1;
                    return;
                }
            }
            for (int j = 0; j < rooms[i].game.nb_spectators; ++j) {
                if (rooms[j].game.players[2 + j].id ==
                    clients[current_client].sock) {
                    // remove spectator from game
                    rooms[i].game.players[j + 2] =
                        rooms[i].game.players[rooms[i].game.nb_spectators - 1];
                    --rooms[i].game.nb_spectators;
                }
            }
        }
    }
}

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

size_t server_client_protocol_write_room_creation_successful(uint8_t *buf,
                                                             uint32_t room_id) {
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

size_t server_client_protocol_write_join_room_successful(
    uint8_t *buf, const char **users, uint8_t nb_users, uint8_t nb_spectators,
    const char **player_bios) {
    uint16_t size = 3;
    buf[2] = JOIN_ROOM_SUCCESSFUL;
    *(uint8_t *)&buf[3] = nb_users;
    *(uint8_t *)&buf[4] = nb_spectators;

    for (int i = 0; i < nb_users + nb_spectators; ++i) {
        strcpy((char *)&buf[size + 2], users[i]);

        size += strlen(users[i]) + 1;

        if (i < nb_users) {
            strcpy((char *)&buf[size + 2], player_bios[i]);

            size += strlen(player_bios[i]) + 1;
        }
    }

    *(uint16_t *)&buf[0] = size;
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

size_t server_client_protocol_write_played(uint8_t *buf, int side,
                                           const Game *game) {
    uint16_t size = BOARD_SIZE + 3;
    *(uint16_t *)&buf[0] = size;
    buf[2] = PLAYED;
    buf[3] = game->players[side].captured;
    buf[4] = game->players[1 - side].captured;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        buf[5 + i] = game->board[(i + BOARD_SIZE / 2 * side) % BOARD_SIZE];
    }

    return size + 2;
}

size_t server_client_protocol_write_invalid_play(uint8_t *buf,
                                                 const char *error_message) {
    uint16_t size = strlen(error_message) + 2;
    *(uint16_t *)&buf[0] = size;
    buf[2] = INVALID_PLAY;

    strcpy((char *)&buf[3], error_message);

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

size_t server_client_protocol_write_game_stopped(uint8_t *buf, uint8_t draw,
                                                 const char *winner) {
    uint16_t size = 1 + sizeof(draw);
    if (!draw) {
        size += 1 + strlen(winner);
        strcpy((char *)&buf[4], winner);
    }
    *(uint16_t *)&buf[0] = size;
    buf[2] = GAME_STOPPED;
    *(uint8_t *)&buf[3] = draw;
    return size + 2;
}

size_t server_client_protocol_write_send_message_to_room(uint8_t *buf,
                                                         const char *username,
                                                         const char *message) {
    uint16_t size = strlen(message) + strlen(username) + 3;
    *(uint16_t *)&buf[0] = size;
    buf[2] = MESSAGE;
    strcpy((char *)&buf[3], username);
    strcpy((char *)&buf[3 + strlen(username) + 1], message);
    return size + 2;
}

void handlers_init(Handlers *handlers) {
    handlers->connect = handle_connect;
    handlers->create_room = handle_create_room;
    handlers->join_room = handle_join_room;
    handlers->leave_room = handle_leave_room;
    handlers->play = handle_play;
    handlers->send_message = handle_send_message;
    handlers->set_biography = handle_set_biography;
}
