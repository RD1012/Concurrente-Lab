
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
    mqd_t qHandlerLlamadas;
    mqd_t qHandlerLinea;
    char buzonLinea[TAMANO_MENSAJES];
    char buffer[TAMANO_MENSAJES+1];

    // Inicia Random
    srand(pid);

    // Verifica los parámetros
    if (argc != 2)
    {
        fprintf(stderr, "Error. Usa: ./exec/linea <id_linea>.\n");
        exit(EXIT_FAILURE);
    }

    // Construye el nombre del buzon de esta linea
    int id_linea = atoi(argv[1]);
    if (id_linea < 0 || id_linea >= NUMLINEAS)
    {
        fprintf(stderr, "Error. id_linea fuera de rango [0..%d].\n", NUMLINEAS - 1);
        exit(EXIT_FAILURE);
    }
    snprintf(buzonLinea, sizeof(buzonLinea), "%s%d", BUZON_LINEAS, id_linea);

    // Abre colas existentes creadas por manager
    qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_WRONLY);
    if (qHandlerLlamadas == (mqd_t)-1)
    {
        fprintf(stderr, "Linea[%d] error abriendo %s: %s\n", pid, BUZON_LLAMADAS, strerror(errno));
        exit(EXIT_FAILURE);
    }

    qHandlerLinea = mq_open(buzonLinea, O_RDONLY);
    if (qHandlerLinea == (mqd_t)-1)
    {
        fprintf(stderr, "Linea[%d] error abriendo %s: %s\n", pid, buzonLinea, strerror(errno));
        mq_close(qHandlerLlamadas);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        int espera = (rand() % 30) + 1;
        printf("Linea [%d] esperando llamada...\n", pid);
        sleep(espera);

        if (mq_send(qHandlerLlamadas, buzonLinea, strlen(buzonLinea) + 1, 0) == -1)
        {
            fprintf(stderr, "Linea [%d] error enviando llamada: %s\n", pid, strerror(errno));
            continue;
        }

        printf("Linea [%d] recibida llamada (%s)...\n", pid, buzonLinea);
        printf("Linea [%d] esperando fin de conversacion...\n", pid);

        ssize_t bytes = mq_receive(qHandlerLinea, buffer, TAMANO_MENSAJES, NULL);
        if (bytes == -1)
        {
            fprintf(stderr, "Linea [%d] error recibiendo fin de llamada: %s\n", pid, strerror(errno));
            continue;
        }

        buffer[bytes] = '\0';
        if (strcmp(buffer, FIN_CONVERSACION) == 0)
        {
            printf("Linea [%d] conversacion finalizada...\n", pid);
        }
    }
    mq_close(qHandlerLinea);
    mq_close(qHandlerLlamadas);
    return EXIT_SUCCESS;
}