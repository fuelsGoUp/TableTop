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
