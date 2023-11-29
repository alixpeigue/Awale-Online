#include "client.h"
#include "protocol.h"
#include "state.h"
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void app(char* address) {
    
    Handlers handlers;
    handlers.connection_successful = &handle_connection_successful;
    handlers.connection_refused = &handle_connection_refused;
    handlers.room_creation_successful = &handle_room_creation_successful;
    handlers.room_creation_refused = &handle_room_creation_refused;
    handlers.join_room_successful = &handle_join_room_successful;
    handlers.join_room_refused = &handle_join_room_refused;
    handlers.played = &handle_played;
    handlers.game_start = &handle_game_start;
    handlers.player_joined_room = &handle_player_joined_room;
    handlers.spectator_joined_room = &handle_spectator_joined_room;
    handlers.game_stopped = &handle_game_stopped;
    handlers.invalid_play = &handle_invalid_play;
    handlers.message = &handle_message;

    State state;
    SOCKET sock = init_connection(address);
    set_current_state(&state, CONNECTING);

    char buffer[BUF_SIZE];

    fd_set rdfs;

    while (1) {
        FD_ZERO(&rdfs);

        /* add STDIN_FILENO */
        FD_SET(STDIN_FILENO, &rdfs);

        /* add the socket */
        FD_SET(sock, &rdfs);

        if (select(sock + 1, &rdfs, NULL, NULL, NULL) == -1) {
            perror("select()");
            exit(errno);
        }

        /* something from standard input : i.e keyboard */
        if (FD_ISSET(STDIN_FILENO, &rdfs)) {
            fgets(buffer, BUF_SIZE - 1, stdin);
            {
                char *p = NULL;
                p = strstr(buffer, "\n");
                if (p != NULL) {
                    *p = 0;
                } else {
                    /* fclean */
                    buffer[BUF_SIZE - 1] = 0;
                }
            }
            handle_user_input(&state, buffer);
        } else if (FD_ISSET(sock, &rdfs)) {
            int n = read_server(buffer, 2);
            uint16_t to_read = *(uint16_t *) buffer;
            int m = read_server(buffer, to_read);
            /* server down */
            if (n == 0 || m == 0) {
                printf("Server disconnected !\n");
                break;
            }
            server_client_protocol_read((uint8_t *) buffer, &handlers, &state);
        }
    }

    end_connection(sock);
}
int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage : %s [address] \n", argv[0]);
        return EXIT_FAILURE;
    }
    printf("Bienvenue sur le jeu d'awale !\n");

    app(argv[1]);  
}
