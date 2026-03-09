#ifndef _LISTA_
#define _LISTA_

typedef struct Nodo
{
  char *valor;
  struct Nodo *pSiguiente;
} TNodo;

typedef struct
{
  TNodo *pPrimero;
} TLista;

void crear(TLista *pLista, const char *valor);
void destruir(TLista *pLista);

// 'insertar' inserta al ppio de la lista.
void insertar(TLista *pLista, const char *valor);
void insertarFinal(TLista *pLista, const char *valor);
void insertarN(TLista *pLista, int index, const char *valor);

// 'elimina' elimina el último de la lista.
void eliminar(TLista *pLista);
void eliminarN(TLista *pLista, int index);

char *getElementoN(TLista *pLista, int index);

void imprimir(TLista *pLista);
int longitud(TLista *pLista);

#endif