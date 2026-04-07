#include <errno.h>
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>


void procesar_argumentos(int argc, char *argv[], int *num_telefonos, int *num_lineas);
void instalar_manejador_senhal();
void manejador_senhal(int senhal);
void iniciar_tabla_procesos(int n_procesos_telefono, int n_procesos_linea);
void crear_procesos(int num_telefonos, int num_lineas);
void lanzar_proceso_telefono(const int indice_tabla);
void lanzar_proceso_linea(const int indice_tabla);
void esperar_procesos();
void terminar_procesos(void);
void terminar_procesos_especificos(struct TProcess_t *tabla_procesos, int num_procesos);
void liberar_recursos();

int g_num_procesos_telefonos = 0;
int g_num_procesos_lineas = 0;
int g_apagado_solicitado = 0;
struct TProcess_t *g_tabla_procesos_telefonos;
struct TProcess_t *g_tabla_procesos_lineas;


// Punto de entrada: inicializa Obelix, crea procesos y gestiona el apagado.
int main(int argc, char *argv[])
{
    // Define variables locales
    int num_telefonos;
    int num_lineas;

    // Procesa los argumentos y los guarda en las dos variables
    procesar_argumentos(argc,argv,&num_telefonos,&num_lineas);

    // Creamos semáforos y memoria compartida
    crear_sem(MUTEXESPERA,1); crear_sem(TELEFONOS,0);crear_sem(LINEAS,0);
    crear_var(LLAMADASESPERA,0);  // No hay llamadas en espera

    // Manejador de Ctrl-C
    instalar_manejador_senhal();
 
    // Crea Tabla para almacenar los pids de los procesos
    iniciar_tabla_procesos(num_telefonos, num_lineas);

    // Tenemos todo
    // Lanzamos los procesos
    crear_procesos(num_telefonos,num_lineas);
  
    // Esperamos a que finalicen las lineas
    esperar_procesos();

    // Matamos los telefonos y cualquier otro proceso restante
    terminar_procesos();

    // Finalizamos Manager
    printf("\n[MANAGER] Terminacion del programa (todos los procesos terminados).\n");
    liberar_recursos();

    return EXIT_SUCCESS;
}

//TODO: Realizar todas las funciones necesarias.
// Valida y convierte los argumentos de entrada del manager.
void procesar_argumentos(int argc, char *argv[], int *num_telefonos, int *num_lineas)
{
    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s <n_telefonos> <n_lineas>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    *num_telefonos = atoi(argv[1]);
    *num_lineas = atoi(argv[2]);

    if (*num_telefonos <= 0 || *num_lineas <= 0)
    {
        fprintf(stderr, "Error: ambos argumentos deben ser enteros positivos.\n");
        exit(EXIT_FAILURE);
    }
}

// Registra el tratamiento de Ctrl-C para apagado de emergencia.
void instalar_manejador_senhal()
{
  if (signal(SIGINT, manejador_senhal) == SIG_ERR)
  {
    fprintf(stderr, "[MANAGER] Error al instalar el manejador se senhal: %s.\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

void manejador_senhal(int sign)
{
    int i;
    (void)sign;
    g_apagado_solicitado = 1;

    for (i = 0; i < g_num_procesos_lineas; i++)
    {
        if (g_tabla_procesos_lineas[i].pid > 0)
        {
            kill(g_tabla_procesos_lineas[i].pid, SIGTERM);
        }
    }

    for (i = 0; i < g_num_procesos_telefonos; i++)
    {
        if (g_tabla_procesos_telefonos[i].pid > 0)
        {
            kill(g_tabla_procesos_telefonos[i].pid, SIGTERM);
        }
    }
}
    
// Reserva e inicializa las tablas globales con los PID de hijos.
void iniciar_tabla_procesos(int n_procesos_telefono, int n_procesos_linea)
{
    int i;
    g_num_procesos_telefonos = n_procesos_telefono;
    g_num_procesos_lineas = n_procesos_linea;

    g_tabla_procesos_telefonos = (struct TProcess_t *)calloc(g_num_procesos_telefonos, sizeof(struct TProcess_t));
    g_tabla_procesos_lineas = (struct TProcess_t *)calloc(g_num_procesos_lineas, sizeof(struct TProcess_t));

    if (g_tabla_procesos_telefonos == NULL || g_tabla_procesos_lineas == NULL)
    {
        fprintf(stderr, "Error reservando memoria para tablas de procesos.\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < g_num_procesos_telefonos; i++)
    {
        g_tabla_procesos_telefonos[i].pid = -1;
        g_tabla_procesos_telefonos[i].clase = CLASE_TELEFONO;
    }

    for (i = 0; i < g_num_procesos_lineas; i++)
    {
        g_tabla_procesos_lineas[i].pid = -1;
        g_tabla_procesos_lineas[i].clase = CLASE_LINEA;
    }
}

// Crea todos los procesos telefono y linea solicitados.
void crear_procesos(int num_telefonos, int num_lineas)
{
    int i;
    for (i = 0; i < num_telefonos; i++)
    {
        lanzar_proceso_telefono(i);
    }

    for (i = 0; i < num_lineas; i++)
    {
        lanzar_proceso_linea(i);
    }
}

// Crea un hijo y ejecuta el binario telefono.
void lanzar_proceso_telefono(const int indice_tabla)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "Error creando proceso telefono: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        execl(RUTA_TELEFONO, RUTA_TELEFONO, (char *)NULL);
        fprintf(stderr, "Error lanzando telefono: %s\n", strerror(errno));
        _exit(EXIT_FAILURE);
    }

    g_tabla_procesos_telefonos[indice_tabla].pid = pid;
}

// Crea un hijo y ejecuta el binario linea.
void lanzar_proceso_linea(const int indice_tabla)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "Error creando proceso linea: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        execl(RUTA_LINEA, RUTA_LINEA, (char *)NULL);
        fprintf(stderr, "Error lanzando linea: %s\n", strerror(errno));
        _exit(EXIT_FAILURE);
    }

    g_tabla_procesos_lineas[indice_tabla].pid = pid;
}

// Espera la finalizacion natural de todos los procesos linea.
void esperar_procesos()
{
    int i;
    int estado_hijo;
    for (i = 0; i < g_num_procesos_lineas; i++)
    {
        if (g_tabla_procesos_lineas[i].pid <= 0)
        {
            continue;
        }

        while (waitpid(g_tabla_procesos_lineas[i].pid, &estado_hijo, 0) == -1)
        {
            if (errno == EINTR)
            {
                if (g_apagado_solicitado)
                {
                    break;
                }
                continue;
            }
            fprintf(stderr, "Error esperando linea %d: %s\n", g_tabla_procesos_lineas[i].pid, strerror(errno));
            break;
        }
    }
}

// Finaliza por senhal y recoge una lista de procesos.
void terminar_procesos_especificos(struct TProcess_t *tabla_procesos, int num_procesos)
{
    int i;
    for (i = 0; i < num_procesos; i++)
    {
        if (tabla_procesos[i].pid <= 0)
        {
            continue;
        }
        kill(tabla_procesos[i].pid, SIGTERM);
    }

    for (i = 0; i < num_procesos; i++)
    {
        if (tabla_procesos[i].pid <= 0)
        {
            continue;
        }
        while (waitpid(tabla_procesos[i].pid, NULL, 0) == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            if (errno == ECHILD)
            {
                break;
            }
            fprintf(stderr, "Error finalizando proceso %d: %s\n", tabla_procesos[i].pid, strerror(errno));
            break;
        }
        tabla_procesos[i].pid = -1;
    }
}

// Termina primero telefonos y despues posibles lineas restantes.
void terminar_procesos(void)
{
    terminar_procesos_especificos(g_tabla_procesos_telefonos, g_num_procesos_telefonos);
    terminar_procesos_especificos(g_tabla_procesos_lineas, g_num_procesos_lineas);
}

// Libera semaforos, memoria compartida y tablas reservadas.
void liberar_recursos()
{
    destruir_sem(MUTEXESPERA);
    destruir_sem(TELEFONOS);
    destruir_sem(LINEAS);
    destruir_var(LLAMADASESPERA);

    free(g_tabla_procesos_telefonos);
    free(g_tabla_procesos_lineas);
}