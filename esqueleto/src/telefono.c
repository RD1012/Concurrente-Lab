
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>

// Modulo principal
int main(int argc,char *argv[]){

    // Define variables locales
    int pid = getpid();
    int llamadas_en_espera;
    sem_t *sem_mutex_espera;
    sem_t *sem_telefonos;
    sem_t *sem_lineas;
    int memoria_llamadas_espera;

    // Define variables locales
    (void)argc;
    (void)argv;
    srand((unsigned int)pid);

    // Coge semáforos y memoria compartida
    sem_mutex_espera = get_sem(MUTEXESPERA);
    sem_telefonos = get_sem(TELEFONOS);
    sem_lineas = get_sem(LINEAS);
    memoria_llamadas_espera = obtener_var(LLAMADASESPERA);

    // Se pone en estado de libre incrementando el número de teléfonos libres
    while(1){

        // Mensaje de Espera
        printf("Telefono[%d] en espera...\n",pid);
        
        signal_sem(sem_telefonos);
        wait_sem(sem_lineas);
        wait_sem(sem_mutex_espera);
        consultar_var(memoria_llamadas_espera, &llamadas_en_espera);
        if (llamadas_en_espera > 0) {
            llamadas_en_espera--;
        }
        modificar_var(memoria_llamadas_espera, llamadas_en_espera);
        signal_sem(sem_mutex_espera);


        // Mensaje de en conversacion
        printf("Telefono[%d] en conversacion... Nº de llamadas en espera: %d\n",pid,llamadas_en_espera);

        // Espera en conversación
        sleep(rand() % 11 + 10);
        
    }

    return EXIT_SUCCESS;
}