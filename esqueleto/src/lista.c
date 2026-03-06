#include <stdio.h>
#include <stdlib.h>
#include <lista.h>

// Crea una lista con un nodo.
void crear(TLista *pLista, int valor)
{
  pLista->pPrimero = malloc(sizeof(TNodo));
  pLista->pPrimero->valor = valor;
  pLista->pPrimero->pSiguiente = NULL;
}

void destruir(TLista *pLista)
{
  TNodo *pAux1;
  struct Nodo *pAux2;

  for (pAux1 = pLista->pPrimero; pAux1 != NULL;)
  {
    pAux2 = pAux1->pSiguiente;
    free(pAux1);
    pAux1 = pAux2;
  }

  free(pLista);
}

// Inserta al ppio de la lista.
void insertar(TLista *pLista, int valor)
{
  TNodo *pNuevo = malloc(sizeof(TNodo));
  if (!pNuevo)
  return;

  pNuevo->valor = valor;
  pNuevo->pSiguiente = pLista->pPrimero;
  pLista->pPrimero = pNuevo;
  
}

void insertarFinal(TLista *pLista, int valor)
{
  TNodo *pNuevo = malloc(sizeof(TNodo));
  if (!pNuevo)
  return;

  pNuevo->valor = valor;
  pNuevo->pSiguiente = NULL;

  if (pLista->pPrimero == NULL)
  {
    pLista->pPrimero = pNuevo;
    return;
  }

  TNodo *pRec = pLista->pPrimero;
  while (pRec->pSiguiente != NULL)
    pRec = pRec->pSiguiente;

  pRec->pSiguiente = pNuevo;
  
}

// Suponemos n = 1, 2, ...
void insertarN(TLista *pLista, int index, int valor)
{
   if (index <= 1 || pLista->pPrimero == NULL)
  {
    insertar(pLista, valor);
    return;
  }

  TNodo *pRec = pLista->pPrimero;
  int i = 1;
  /* avanzar hasta la posición index-1 o hasta el último */
  while (pRec->pSiguiente != NULL && i < index - 1)
  {
    pRec = pRec->pSiguiente;
    i++;
  }

  TNodo *pNuevo = malloc(sizeof(TNodo));
  if (!pNuevo)
    return;
  pNuevo->valor = valor;
  pNuevo->pSiguiente = pRec->pSiguiente;
  pRec->pSiguiente = pNuevo;
}

// Elimina el primer elemento de la lista.
void eliminar(TLista *pLista)
{
   if (pLista->pPrimero == NULL)
    return;

  TNodo *pRec = pLista->pPrimero;
  if (pRec->pSiguiente == NULL)
  {
    free(pRec);
    pLista->pPrimero = NULL;
    return;
  }

  /* buscar penúltimo */
  while (pRec->pSiguiente->pSiguiente != NULL)
    pRec = pRec->pSiguiente;

  free(pRec->pSiguiente);
  pRec->pSiguiente = NULL;
}

void eliminarN(TLista *pLista, int index)
{
  if (pLista->pPrimero == NULL || index < 1)
    return;

  if (index == 1)
  {
    TNodo *pTmp = pLista->pPrimero;
    pLista->pPrimero = pTmp->pSiguiente;
    free(pTmp);
    return;
  }

  TNodo *pRec = pLista->pPrimero;
  int i = 1;
  while (pRec->pSiguiente != NULL && i < index - 1)
  {
    pRec = pRec->pSiguiente;
    i++;
  }

  if (pRec->pSiguiente == NULL)
    return; /* fuera de rango */

  TNodo *pTmp = pRec->pSiguiente;
  pRec->pSiguiente = pTmp->pSiguiente;
  free(pTmp);
}

int getElementoN(TLista *pLista, int index)
{
  if (pLista->pPrimero == NULL || index < 1)
    return 0;

  TNodo *pRec = pLista->pPrimero;
  int i = 1;
  while (pRec != NULL && i < index)
  {
    pRec = pRec->pSiguiente;
    i++;
  }

  if (pRec == NULL)
    return 0;

  return pRec->valor;
}

void imprimir(TLista *pLista)
{
  TNodo *pRec = pLista->pPrimero;
  while (pRec != NULL)
  {
    printf("%d ", pRec->valor);
    pRec = pRec->pSiguiente;
  }
  printf("\n");
}

int longitud(TLista *pLista)
{
  int cnt = 0;
  TNodo *pRec = pLista->pPrimero;
  while (pRec != NULL)
  {
    cnt++;
    pRec = pRec->pSiguiente;
  }
  return cnt;
}