#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "[PROCESADOR] Uso: %s <archivo_texto> <patron>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *nombre_archivo = argv[1];
    char *patron = argv[2];
    // Abrir el archivo para lectura
    FILE *fp = fopen(nombre_archivo, "r");
    if (!fp)
    {
        perror("[PROCESADOR] Error al abrir archivo");
        return EXIT_FAILURE;
    }

    char linea[4096];
    int numero_linea = 1;
    // Leer el archivo línea por línea
    while (fgets(linea, sizeof(linea), fp) != NULL)
    {
      // Eliminar el salto de línea al final de la línea
        if (strstr(linea, patron) != NULL)
        {
            printf("[PROCESADOR %d] Patron '%s' encontrado en linea %d\n",
                   getpid(), patron, numero_linea);
        }

        numero_linea++;
    }
    // Cerrar el archivo
    fclose(fp);

    return EXIT_SUCCESS;
}