#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024

static int conectado = 1;

void enviar_mensagem(int socket, const char *mensagem)
{
    size_t total = 0;
    size_t tamanho = strlen(mensagem);

    while (total < tamanho) {
        ssize_t enviados = send(socket, mensagem + total, tamanho - total, 0);

        if (enviados <= 0) {
            conectado = 0;
            return;
        }

        total += (size_t)enviados;
    }
}

void *receber_mensagens(void *arg)
{
    int socket = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        ssize_t bytes = recv(socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes <= 0) {
            conectado = 0;
            printf("\nServidor desconectado.\n");
            break;
        }

        buffer[bytes] = '\0';
        printf("\n%s", buffer);
        fflush(stdout);
    }

    return NULL;
}

void rolar_dado(int socket)
{
    int lados;
    int resultado;

    printf("Quantos lados o dado possui? ");
    scanf("%d", &lados);
    getchar();

    if (lados <= 0) {
        printf("Numero de lados invalido.\n");
        return;
    }

    resultado = (rand() % lados) + 1;

    char mensagem[BUFFER_SIZE];
    snprintf(mensagem, sizeof(mensagem),
             "Rolou d%d: resultado = %d\n", lados, resultado);

    enviar_mensagem(socket, mensagem);
    printf("%s", mensagem);
}

int main(void)
{
    int client_socket;
    struct sockaddr_in server_address;
    pthread_t thread_receber;

    srand((unsigned int)time(NULL));

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Endereco invalido");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (struct sockaddr *)&server_address,
                sizeof(server_address)) < 0) {
        perror("Erro ao conectar");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Conectado ao servidor!\n");

    if (pthread_create(&thread_receber, NULL, receber_mensagens,
                       &client_socket) != 0) {
        perror("Erro ao criar thread de recebimento");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    pthread_detach(thread_receber);

    while (conectado) {
        int opcao;
        char buffer[BUFFER_SIZE];

        printf("\n===== VIRTUAL TABLETOP =====\n");
        printf("1 - Enviar mensagem\n");
        printf("2 - Rolar dado\n");
        printf("3 - Sair\n");
        printf("Escolha: ");
        fflush(stdout);

        if (scanf("%d", &opcao) != 1) {
            int caractere;
            while ((caractere = getchar()) != '\n' && caractere != EOF) {}
            printf("Opcao invalida.\n");
            continue;
        }
        getchar();

        if (opcao == 1) {
            printf("Mensagem: ");
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                break;
            }

            buffer[strcspn(buffer, "\n")] = '\0';
            strcat(buffer, "\n");
            enviar_mensagem(client_socket, buffer);
        } else if (opcao == 2) {
            rolar_dado(client_socket);
        } else if (opcao == 3) {
            printf("Desconectando...\n");
            break;
        } else {
            printf("Opcao invalida.\n");
        }
    }

    conectado = 0;
    shutdown(client_socket, SHUT_RDWR);
    close(client_socket);
    return 0;
}
