#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Mensagem enviada com sucesso!\n";

    // Criando o socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Erro ao criar o socket");
        exit(EXIT_FAILURE);
    }

    // Configurando o endereço do servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Ligando o socket ao endereço e porta
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Erro ao associar o socket");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Colocando o socket em modo de escuta
    if (listen(server_fd, 3) < 0) {
        perror("Erro ao escutar");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor HTTP iniciado na porta %d...\n", PORT);

    // Loop para aceitar conexões
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Erro ao aceitar conexão");
            continue;
        }

        // Lendo a solicitação do cliente (opcional, dependendo do uso)
        read(new_socket, buffer, BUFFER_SIZE);
        printf("Requisição recebida:\n%s\n", buffer);

        // Enviando a resposta HTTP
        write(new_socket, response, strlen(response));

        // Fechando o socket do cliente
        //close(new_socket);
    }

    // Fechando o socket do servidor
    close(server_fd);

    return 0;
}