
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
    
	//TODO: Esquema especificado en la práctica.
	
    // Coge semáforos y memoria compartida
    (void)argc;
    (void)argv;
    srand((unsigned int)pid);
    sem_mutex_espera = get_sem(MUTEXESPERA);
    sem_telefonos = get_sem(TELEFONOS);
    sem_lineas = get_sem(LINEAS);
    memoria_llamadas_espera = obtener_var(LLAMADASESPERA);


    // Realiza una espera entre 1..60 segundos
    printf("Linea [%d] esperando llamada...\n",pid);
    sleep(rand() % 30 + 1);

    //Aumenta las llamadas en espera
    wait_sem(sem_mutex_espera);
    consultar_var(memoria_llamadas_espera, &llamadas_en_espera);
    llamadas_en_espera++;
    modificar_var(memoria_llamadas_espera, llamadas_en_espera);
    signal_sem(sem_mutex_espera);


    // Espera telefono libre
    printf("Linea [%d] esperando telefono libre...Nº Llamadas en espera: %d\n",pid,llamadas_en_espera);
    wait_sem(sem_telefonos);

    // Lanza la llamada
    printf("Linea [%d] desviando llamada a un telefono...\n",pid);
    signal_sem(sem_lineas);

    sem_close(sem_mutex_espera);
    sem_close(sem_telefonos);
    sem_close(sem_lineas);
    close(memoria_llamadas_espera);

    return EXIT_SUCCESS;
}