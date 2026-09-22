#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void enviar_mensagem(int socket, char *mensagem)
{
    write(socket, mensagem, strlen(mensagem));
}

void rolar_dado(int socket)
{
    int lados;
    int resultado;

    printf("Quantos lados o dado possui? ");
    scanf("%d", &lados);

    if (lados <= 0) {
        printf("Numero de lados invalido.\n");
        return;
    }

    resultado = (rand() % lados) + 1;

    char mensagem[BUFFER_SIZE];

    snprintf(
        mensagem,
        sizeof(mensagem),
        "Rolou d%d: resultado = %d\n",
        lados,
        resultado
    );

    enviar_mensagem(socket, mensagem);

    printf("%s", mensagem);
}

int main()
{
    int client_socket;

    struct sockaddr_in server_address;

    char buffer[BUFFER_SIZE];

    srand(time(NULL));

    /*
     * Criando socket
     */
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    /*
     * Configurando endereco do servidor
     */
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    /*
     * IP do servidor
     */
    if (inet_pton(
            AF_INET,
            "127.0.0.1",
            &server_address.sin_addr
        ) <= 0) {

        perror("Endereco invalido");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    /*
     * Conectando ao servidor
     */
    if (connect(
            client_socket,
            (struct sockaddr *)&server_address,
            sizeof(server_address)
        ) < 0) {

        perror("Erro ao conectar");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Conectado ao servidor!\n");

    int opcao;

    while (1) {

        printf("\n");
        printf("===== VIRTUAL TABLETOP =====\n");
        printf("1 - Enviar mensagem\n");
        printf("2 - Rolar dado\n");
        printf("3 - Sair\n");
        printf("Escolha: ");

        scanf("%d", &opcao);

        /*
         * Limpa o '\n' deixado pelo scanf
         */
        getchar();

        if (opcao == 1) {

            printf("Mensagem: ");

            fgets(buffer, BUFFER_SIZE, stdin);

            buffer[strcspn(buffer, "\n")] = '\0';

            enviar_mensagem(client_socket, buffer);

        }

        else if (opcao == 2) {

            rolar_dado(client_socket);

        }

        else if (opcao == 3) {

            printf("Desconectando...\n");
            break;

        }

        else {

            printf("Opcao invalida.\n");
        }
    }

    close(client_socket);

    return 0;
}
