#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "client.h"
#include "game.h"
#include "protocol.h"
#include "server.h"

static void init(void) {
#ifdef WIN32
    WSADATA wsa;
    int err = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (err < 0) {
        puts("WSAStartup failed !");
        exit(EXIT_FAILURE);
    }
#endif
}

static void end(void) {
#ifdef WIN32
    WSACleanup();
#endif
}

int nb_clients = 0;
int current_client = -1;
Client clients[MAX_CLIENTS];
char buffer[BUF_SIZE];
int payload_size;

static void app(void) {
    SOCKET sock = init_connection();
    /* the index for the array */
    int max = sock;
    /* an array for all clients */
    Handlers handlers;
    handlers_init(&handlers);

    fd_set rdfs;

    while (1) {
        int i = 0;
        FD_ZERO(&rdfs);

        /* add STDIN_FILENO */
        FD_SET(STDIN_FILENO, &rdfs);

        /* add the connection socket */
        FD_SET(sock, &rdfs);

        /* add socket of each client */
        for (i = 0; i < nb_clients; i++) {
            FD_SET(clients[i].sock, &rdfs);
        }

        if (select(max + 1, &rdfs, NULL, NULL, NULL) == -1) {
            perror("select()");
            exit(errno);
        }

        /* something from standard input : i.e keyboard */
        if (FD_ISSET(STDIN_FILENO, &rdfs)) {
            /* stop process when type on keyboard */
            break;
        } else if (FD_ISSET(sock, &rdfs)) {
            /* new client */
            SOCKADDR_IN csin = {0};
            socklen_t sinsize = sizeof csin;
            int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
            if (csock == SOCKET_ERROR) {
                perror("accept()");
                continue;
            }

            /* after connecting the client sends its name */
            // if (read_client(csock, buffer) == -1) {
            //     /* disconnected */
            //     continue;
            // }

            current_client = nb_clients;

            /* what is the new maximum fd ? */
            max = csock > max ? csock : max;

            FD_SET(csock, &rdfs);

            clients[current_client].sock = csock;
            clients[current_client].name[0] = '\0';
            nb_clients++;
            fprintf(stderr, "TEST ADD SOCKET\n");
        } else {
            int i = 0;
            for (i = 0; i < nb_clients; i++) {
                /* a client is talking */
                if (FD_ISSET(clients[i].sock, &rdfs)) {
                    Client client = clients[i];
                    current_client = i;
                    int c = read_client(clients[i].sock, buffer);
                    /* client disconnected */
                    if (c == 0) {
                        closesocket(clients[i].sock);
                        remove_client(clients, i, &nb_clients);
                        fprintf(stderr, "TEST %s disconnected\n",
                                clients[current_client].name);
                        strncpy(buffer, client.name, BUF_SIZE - 1);
                        strncat(buffer, " disconnected !",
                                BUF_SIZE - strlen(buffer) - 1);
                        // send_message_to_all_clients(clients, client,
                        // nb_clients,
                        //                             buffer, 1);
                    } else {
                        client_server_protocol_read((const uint8_t *)buffer,
                                                    &handlers);
                        fprintf(stderr, "TEST %s read\n",
                                clients[current_client].name);

                        /* send_message_to_all_clients(clients, client, actual,
                                                    buffer, 0);
                                                    */
                        write_client(client.sock, buffer, payload_size);
                        fprintf(stderr, "TEST %s wrote\n",
                                clients[current_client].name);
                    }
                    break;
                }
            }
        }
    }

    clear_clients(clients, nb_clients);
    end_connection(sock);
}

static void clear_clients(Client *clients, int actual) {
    int i = 0;
    for (i = 0; i < actual; i++) {
        closesocket(clients[i].sock);
    }
}

static void remove_client(Client *clients, int to_remove, int *actual) {
    /* we remove the client in the array */
    memmove(clients + to_remove, clients + to_remove + 1,
            (*actual - to_remove - 1) * sizeof(Client));
    /* number client - 1 */
    (*actual)--;
}

// static void send_message_to_all_clients(Client *clients, Client sender,
//                                         int actual, const char *buffer,
//                                         char from_server) {
//     int i = 0;
//     char message[BUF_SIZE];
//     message[0] = 0;
//     for (i = 0; i < actual; i++) {
//         /* we don't send message to the sender */
//         if (sender.sock != clients[i].sock) {
//             if (from_server == 0) {
//                 strncpy(message, sender.name, BUF_SIZE - 1);
//                 strncat(message, " : ", sizeof message - strlen(message) -
//                 1);
//             }
//             strncat(message, buffer, sizeof message - strlen(message) - 1);
//             write_client(clients[i].sock, message, payload_size);
//         }
//     }
// }

static int init_connection(void) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN sin = {0};

    if (sock == INVALID_SOCKET) {
        perror("socket()");
        exit(errno);
    }

    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(PORT);
    sin.sin_family = AF_INET;

    if (bind(sock, (SOCKADDR *)&sin, sizeof sin) == SOCKET_ERROR) {
        perror("bind()");
        exit(errno);
    }

    if (listen(sock, MAX_CLIENTS) == SOCKET_ERROR) {
        perror("listen()");
        exit(errno);
    }

    return sock;
}

static void end_connection(int sock) { closesocket(sock); }

static int read_client(SOCKET sock, char *buffer) {
    int n = 0;

    if ((n = recv(sock, buffer, 2, 0)) < 0) {
        perror("recv()");
        /* if recv error we disonnect the client */
        n = 0;
    }

    uint16_t size = *(uint16_t *)buffer;

    if ((n = recv(sock, buffer, size, 0)) < 0) {
        perror("recv()");
        /* if recv error we disonnect the client */
        n = 0;
    }

    buffer[n] = 0;

    return n;
}

static void write_client(SOCKET sock, const char *buffer, int size) {
    if (send(sock, buffer, size, 0) < 0) {
        perror("send()");
        exit(errno);
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));
    init();

    app();

    end();

    return EXIT_SUCCESS;
}
