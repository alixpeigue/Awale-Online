#ifndef STATE_H
#define STATE_H

#include "server_client_protocol.h"
#include <stdint.h>
#include <stdio.h>
typedef enum {
    CONNECTING,
    CONNECTED,
    WAITING_CREATE_ROOM,
    WAITING_JOIN_ROOM_ID,
    WAITING_SPECTATE_ROOM_ID,
    IN_ROOM,
    IN_GAME,
    SPECTATING
} State;

void set_current_state(State *state, State new_state);

void handle_input(State *state, const char *in);

void handle_connection_successful(State *state);

void handle_connection_refused(State *state, const char *error_message);

void handle_room_creation_successful(State *state, uint32_t room_id);

void handle_room_creation_refused(State *state, const char *error_message);

void handle_join_room_successful(State *state, uint8_t nb_users,
                                 const char **users);

void handle_join_room_refused(State *state, const char *error_message);

void handle_spectate_room_successful(State *state);

void handle_spectate_room_refused(State *state, const char *error_message);

void handle_played(State *state, uint8_t pos);

void handle_game_start(State *state, uint8_t pos);

void handle_player_joined_room(State *state, const char *username);

void handle_spectator_joined_room(State *state, const char *username);

void handle_game_stopped(State *state, uint8_t winner);

#endif
