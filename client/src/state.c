#include "state.h"

void set_current_state(State *state, State new_state) { *state = new_state; }

void handle_connection_successful(State *state) {
    if (*state == CONNECTING) {
        printf("You are successfully connected !\n");
        *state = CONNECTED;
    }
}

void handle_connection_refused(State *state, char *error_message) {
    if (*state == CONNECTING) {
        printf("Connection refused : %s\n", error_message);
    }
}

void handle_room_creation_successful(State *state, uint32_t room_id) {
    if (*state == CONNECTED) {
        printf("Room successfully created with id %x\n", room_id);
        *state = IN_ROOM;
    }
}

void handle_room_creation_refused(State *state, char *error_message) {
    if (*state == CONNECTED) {
        printf("Couldn't create room : %s\n", error_message);
    }
}

void handle_join_room_successful(State *state, uint8_t nb_users, char **users) {
    if (*state == CONNECTED) {
        printf("Room joined\nUsers in room spectating room : \n");
        for (uint8_t i = 0; i < nb_users; ++i) {
            printf(" - %s\n", users[0]);
        }
        *state = IN_ROOM;
    }
}

void handle_join_room_refused(State *state, char *error_message) {
    if (*state == CONNECTED) {
        printf("Could not join room : %s\n", error_message);
    }
}

void handle_spectate_room_successful(State *state) {
    if (*state == CONNECTED) {
        printf("Successfilly joined room\n");
        *state = SPECTATING;
    }
}

void handle_spectate_room_refused(State *state, char *error_message) {
    if (*state == CONNECTED) {
        printf("Could not spectate room : %s\n", error_message);
    }
}

void handle_played(State *state, uint8_t pos) {
    if (*state == IN_GAME) {
        printf("Played : %d\n", pos);
    } else if (*state == SPECTATING) {
        printf("(Spectating) Played : %d\n", pos);
    }
}

void handle_game_start(State *state, uint8_t pos) {
    if (*state == IN_ROOM) {
        printf("You start at position %d", pos);
        *state = IN_GAME;
    }
}

void handle_player_joined_room(State *state, char *username) {
    if (*state == IN_ROOM || *state == SPECTATING) {
        printf("Second player joined : %s\n", username);
    }
}

void handle_spectator_joined_room(State *state, char *username) {
    if (*state == IN_ROOM || *state == IN_GAME) {
        printf("User %s is now spectating !\n", username);
    }
}

void handle_game_stopped(State *state, uint8_t winner) {
    if (*state == IN_GAME || *state == SPECTATING || *state == IN_ROOM) {
        if (winner) {
            printf("Game ended ! Player %d won\n", winner);
        } else {
            printf("Room is closed\n");
        }
        *state = CONNECTED;
    }
}
