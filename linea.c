#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>
#include <definitions.h>

int main(int argc, char *argv[]) {

    int pid = getpid();
    mqd_t qHandlerLlamadas;
    mqd_t qHandlerLinea;
    char buzonLinea[TAMANO_MENSAJES];
    char buffer[TAMANO_MENSAJES + 1];

    srand(pid);

    if (argc != 2) {
        fprintf(stderr, "Error. Usa: ./exec/linea <cola_linea_llamante>\n");
        exit(EXIT_FAILURE);
    }

    strcpy(buzonLinea, argv[1]);

    qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_WRONLY);
    if (qHandlerLlamadas == (mqd_t)-1) {
        perror("Error abriendo buzon de llamadas");
        exit(EXIT_FAILURE);
    }

    qHandlerLinea = mq_open(buzonLinea, O_RDONLY);
    if (qHandlerLinea == (mqd_t)-1) {
        perror("Error abriendo buzon de linea");
        mq_close(qHandlerLlamadas);
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Linea[%d] esperando llamada...\n", pid);
        fflush(stdout);

        sleep((rand() % 30) + 1);

        if (mq_send(qHandlerLlamadas, buzonLinea, strlen(buzonLinea) + 1, 0) == -1) {
            perror("Error enviando llamada");
            break;
        }

        printf("Linea[%d] recibida llamada (%s)...\n", pid, buzonLinea);
        printf("Linea[%d] esperando fin de conversacion...\n", pid);
        fflush(stdout);

        memset(buffer, 0, sizeof(buffer));

        if (mq_receive(qHandlerLinea, buffer, TAMANO_MENSAJES, NULL) == -1) {
            perror("Error recibiendo fin de conversacion");
            break;
        }

        if (strcmp(buffer, FIN_CONVERSACION) == 0) {
            printf("Linea[%d] conversacion finalizada...\n", pid);
            fflush(stdout);
        }
    }

    mq_close(qHandlerLlamadas);
    mq_close(qHandlerLinea);

    return EXIT_SUCCESS;
}