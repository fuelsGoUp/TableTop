#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 2

int main() {
    int server_fd;
    int clients[MAX_CLIENTS] = {0};

    struct sockaddr_in address;

    char buffer[BUFFER_SIZE];
    char saida_chat[BUFFER_SIZE];

    // Criando socket do servidor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    // Configurando endereço
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Erro no bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 2) < 0) {
        perror("Erro no listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor iniciado na porta %d\n", PORT);

    while (1) {

        fd_set readfds;

        FD_ZERO(&readfds);

        // Coloca o socket do servidor no conjunto
        FD_SET(server_fd, &readfds);

        int max_fd = server_fd;

        // Coloca os clientes no conjunto
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] > 0) {
                FD_SET(clients[i], &readfds);

                if (clients[i] > max_fd) {
                    max_fd = clients[i];
                }
            }
        }

        // Espera algum socket receber dados
        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("Erro no select");
            break;
        }

        /*
         * Verifica se alguém está tentando
         * estabelecer uma nova conexão.
         */
        if (FD_ISSET(server_fd, &readfds)) {

            int new_socket = accept(server_fd, NULL, NULL);

            if (new_socket < 0) {
                perror("Erro no accept");
                continue;
            }

            // Procurando espaço para o cliente
            int added = 0;

            for (int i = 0; i < MAX_CLIENTS; i++) {

                if (clients[i] == 0) {

                    clients[i] = new_socket;

                    printf("Cliente %d conectado.\n", i + 1);

                    added = 1;
                    break;
                }
            }

            // Se já temos dois clientes
            if (!added) {
                printf("Servidor cheio.\n");
                close(new_socket);
            }
        }

        /*
         * Verifica se algum cliente enviou mensagem.
         */
        for (int i = 0; i < MAX_CLIENTS; i++) {

            int client = clients[i];

            if (client == 0)
                continue;

            if (FD_ISSET(client, &readfds)) {

                memset(buffer, 0, BUFFER_SIZE);

                int bytes = read(client, buffer, BUFFER_SIZE - 1);

                if (bytes <= 0) {

                    printf("Cliente %d desconectou.\n", i + 1);

                    close(client);
                    clients[i] = 0;

                    continue;
                }

                buffer[bytes] = '\0';

                printf("Cliente %d: %s\n", i + 1, buffer);

                /*
                 * Envia a mensagem para o OUTRO cliente.
                 */
                for (int j = 0; j < MAX_CLIENTS; j++) {

                    if (j != i && clients[j] != 0) {

                        snprintf(saida_chat, sizeof(saida_chat), "Usuario %d:", j);
                        strcat(saida_chat, buffer);

                        write(
                            clients[j],
                            saida_chat,
                            strlen(saida_chat)
                        );
                    }
                }
            }
        }
    }

    close(server_fd);

    return 0;
}