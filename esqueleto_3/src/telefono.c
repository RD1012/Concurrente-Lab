
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <mqueue.h>
#include <definitions.h>

// Modulo principal
int main(int argc,char *argv[]){

    // Define variables locales
    int pid = getpid();
    srand(pid);
    mqd_t qHandlerLlamadas;
    mqd_t qHandlerLinea;
    char buzonLinea[TAMANO_MENSAJES];

    qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_RDONLY);
    if (qHandlerLlamadas == (mqd_t)-1)
    {
        fprintf(stderr, "Telefono[%d] error abriendo %s: %s\n", pid, BUZON_LLAMADAS, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        printf("Telefono [%d] en espera...\n", pid);

        ssize_t bytes = mq_receive(qHandlerLlamadas, buzonLinea, TAMANO_MENSAJES, NULL);
        if (bytes == -1)
        {
            fprintf(stderr, "Telefono [%d] error recibiendo llamada: %s\n", pid, strerror(errno));
            continue;
        }
        buzonLinea[bytes] = '\0';

        printf("Telefono [%d] en conversacion de llamada desde Linea: %s\n", pid, buzonLinea);

        int duracion = (rand() % 11) + 10; // 10..20
        sleep(duracion);

        qHandlerLinea = mq_open(buzonLinea, O_WRONLY);
        if (qHandlerLinea == (mqd_t)-1)
        {
            fprintf(stderr, "Telefono [%d] error abriendo %s: %s\n", pid, buzonLinea, strerror(errno));
            continue;
        }

        if (mq_send(qHandlerLinea, FIN_CONVERSACION, strlen(FIN_CONVERSACION) + 1, 0) == -1)
        {
            fprintf(stderr, "Telefono [%d] error notificando fin llamada: %s\n", pid, strerror(errno));
            mq_close(qHandlerLinea);
            continue;
        }

        printf("Telefono [%d] ha colgado la llamada. %s\n", pid, buzonLinea);
        mq_close(qHandlerLinea);
    }

    mq_close(qHandlerLlamadas);

    return EXIT_SUCCESS;
}