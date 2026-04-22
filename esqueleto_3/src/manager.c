#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include <mqueue.h>
#include <definitions.h>

void crear_buzones();
void instalar_manejador_senhal();
void manejador_senhal(int sign);
void iniciar_tabla_procesos(int n_procesos_telefono, int n_procesos_linea);
void crear_procesos(int numTelefonos, int numLineas);
void lanzar_proceso_telefono(const int indice_tabla);
void lanzar_proceso_linea(const int indice_tabla);
void esperar_procesos();
void terminar_procesos(void);
void terminar_procesos_especificos(struct TProcess_t *process_table, int process_num);
void liberar_recursos();

int g_telefonosProcesses = 0;
int g_lineasProcesses = 0;
struct TProcess_t *g_process_telefonos_table;
struct TProcess_t *g_process_lineas_table;
mqd_t qHandlerLlamadas;
mqd_t qHandlerLineas[NUMLINEAS];


int main(int argc, char *argv[])
{
    
    // Creamos los buzones
    crear_buzones();

    // Manejador de Ctrl-C
    instalar_manejador_senhal();
 
    // Crea Tabla para almacenar los pids de los procesos
    iniciar_tabla_procesos(NUMTELEFONOS,NUMLINEAS);

    // Tenemos todo
    // Lanzamos los procesos
    crear_procesos(NUMTELEFONOS,NUMLINEAS);
  
    // Esperamos a que finalicen las lineas
    esperar_procesos();

    // Matamos los telefonos y cualquier otro proceso restante
    terminar_procesos();


    // Finalizamos Manager
    printf("\n[MANAGER] Terminacion del programa (todos los procesos terminados).\n");
    liberar_recursos();

    return EXIT_SUCCESS;
}

void crearbuzones() {
    struct mq_attr attr;
    char nombre_buzon[64];

    attr.mq_flags = 0;
    attr.mq_maxmsg = NUMLINEAS; 
    attr.mq_msgsize = TAMANO_MENSAJES; 
    attr.mq_curmsgs = 0;

    mq_unlink(BUZON_LLAMADAS); 
    qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_CREAT | O_RDWR, 0644, &attr);

    for (int i = 0; i < NUMLINEAS; i++) {
        sprintf(nombre_buzon, "%s%d", BUZON_LINEAS, i); 
        mq_unlink(nombre_buzon);
        qHandlerLineas[i] = mq_open(nombre_buzon, O_CREAT | O_RDWR, 0644, &attr);
    }
}

void instalar_manejador_senhal(){
    signal(SIGINT, manejador_senhal);
}

void manejador_senhal(int sign) {
    terminar_procesos();
    liberar_recursos();

    printf("\n[MANAGER] Finalizacion del programa (Ctrl+C)\n");
    exit(0);
}

void iniciar_tabla_procesos(int n_tele, int n_line){
g_telefonosProcesses = n_tele;
g_lineasProcesses = n_line;
g_process_telefonos_table = malloc(n_tele * sizeof(struct TProcess_t));
g_process_lineas_table = malloc(n_line * sizeof(struct TProcess_t));

}

void crear_procesos(int numTele, int numLine){
    //Creamos primeramente las lineas y luego los telefonos
    for ( int i = 0; i < numLine; i++){
        lanzar_proceso_linea(i);
    }
    printf("[Manager] %d numero de lineas creadas.\n", numLine);

    for (int i = 0; i < numTele; i++){
        lanzar_proceso_telefono(i);
    }
    printf("[Manager] %d numero de telefonos creados", numTele);
}

void lanzar_proceso_linea(int indice){
    pid_t pid = fork();
    if (pid == 0){
        char arg_id[10];
        sprintf(arg_id, "%d", indice);
        execl(RUTA_LINEA, CLASE_LINEA, arg_id, NULL);
        exit(EXIT_FAILURE);
    } else {
        g_process_lineas_table[indice].pid = pid;
        g_process_lineas_table[indice].pid = CLASE_LINEA;
    }

}

void lanzar_proceso_telefono(int indice){
    pid_t pid = fork();
    if (pid == 0){
        execl(RUTA_TELEFONO, CLASE_TELEFONO, NULL);
        exit(EXIT_FAILURE);
    } else {
        g_process_telefonos_table[indice].pid = pid;
        g_process_telefonos_table[indice].clase = CLASE_TELEFONO;

    }
}

void esperar_procesos(){
    printf("[Manager] Terminando con cualquier proceso pendiente.....\n");
    terminar_procesos_especificos(g_process_lineas_table, g_lineasProcesses);
    terminar_procesos_especificos(g_process_telefonos_table, g_telefonosProcesses);
}

void terminar_procesos_especificos(struct TProcess_t *table, int num){
    
}