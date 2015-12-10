#include <stdlib.h>
#include <stdio.h>
#include "TADlista.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::                               ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::    T.A.D. LISTAS ENLAZADAS    ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::                               ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//------------------------------------------------------TYPEDEF's y declaracion estructuras-------------------------------------------------------------
//en TADlista.h
/*
typedef int tipoElemento;       //Define a int como tipoElemento, así, cada vez que escribamos tipoElemento, en verdad estamos escribiendo int
                                //Con cambiar el int por otro tipo, tendríamos otra implementacion, aunque habría que cambiar métodos de búsqueda
                                //por valor del elemento.
                                //Se trata del tipo de dato que vamos a manejar.

typedef struct tipoCelda        //Es la definición de nuestra celda, la "unidad" que vamos a manejar, de lo que van a estar hechas las listas.
{
	tipoElemento elemento;        //es el campo de información de nuestra celda :) es un elemento del tipo tipoElemento
	struct tipoCelda * sig;       //es el puntero que apunta a la celda que nos sucede
} tipoCelda;                    //Como es un typedef, para definir una celda, nos basta con escribir tipoCelda.


typedef struct                  //Define la Lista como una estructura con dos punteros tipoCelda (punteros a celdas)
{
	tipoCelda * cabecera;         //uno a la primera celda de la lista
  tipoCelda * ultimo;           //otro a la última celda de la lista
} Lista;                        //Como es un typedef, para declarar una lista de celdas bastará con usar Lista.

typedef tipoCelda *tipoPosicion;    //Al puntero al tipoCelta lo llamamos tipoPosicion
*/

//--------------------------------------------------------Cabeceras de funciones-------------------------------------------------------------------------
  //Función de inicialización
//en TADlista.h
//en TADlista.h
/*
void iniciarLista(Lista *l);                            //FUNCION QUE CREA UNA NUEVA LISTA

//Funciones de localización
tipoPosicion primero(Lista l);                          //DEVUELVE UN PUNTERO A LA CELDA (un tipoPosicion) CORRESPONDIENTE AL PRIMER ELEMENTO DE LA LISTA l
tipoPosicion siguiente(tipoPosicion p,Lista l);         //DEVUELVE UN tipoPosicion CORRESPONDIENTE AL ELEMENTO SIGUIENTE A p DE LA LISTA l
tipoPosicion anterior(tipoPosicion p, Lista l);         //DEVUELVE UN tipoPosicion CORRESPONDIENTE AL ELEMENTO ANTERIOR A p DE LA LISTA l
tipoPosicion fin(Lista l);                              //DEVUELVE UN tipoPosicion CORRESPONDIENTE AL ELEMENTO FINAL DE LA LISTA l
tipoPosicion localiza (tipoElemento x, Lista l);        //LOCALIZA EL ELEMENTO x EN LA LISTA l , Y DEVUELVE SU POSICIÓN
tipoElemento recupera(tipoPosicion p, Lista l);         //LOCALIZA EL EL ELEMENTO x EN LA POSICIÓN x DE LA LISTA l Y DEVUELVE SU POSICIÓN


  //Funciones de modificación
int inserta(tipoElemento x, tipoPosicion p, Lista *l);  //INSERTA UN ELEMENTO (de tipoElemento) x EN LA POSICIÓN p DE LA LISTA l
int elimina(tipoPosicion p, Lista *l);                  //ELIMINA EL ELEMENTO EN LA POSICIÓN p DE LA LISTA l

  //Funciones auxiliares
tipoPosicion anula(Lista *l);                           //ANULA LA LISTA::::::::::::::::WTF????
void imprimeLista(Lista l);                             //IMPRIME POR PANTALLA LA LISTA l
*/

//--------------------------------------------------------Implementacion de las funciones del TAD--------------------------------------------------------

  //-----Función de inicialización-----

void iniciarLista(Lista *l)							            //Creación de una nueva lista
{

    tipoPosicion ficticio;							            //declaracion de un nuevo elemento de tipoPosicion, el elemento ficticio
    ficticio = malloc(sizeof(tipoPosicion));		    //reserva de espacio para el elemento ficticio

    ficticio->elemento=-999;						            //copiamos el numero -999 al campo elemento (en este caso int), del nodo ficticio
    l->cabecera=ficticio;							              //ponemos al ficticio como cabecera de la lista
    l->ultimo=ficticio;								              //como la lista está recién creada, el ficticio también es el ultimo (cola) de la lista
    ficticio->sig=NULL;								              //indicamos que el siguiente al ficticio no existe (debido a que la lista está recien creada)

}



  //-----Funciones de localización

tipoPosicion primero(Lista *l)     //devuelve "la posicion" del primer elemento de la lista que se le pasa (l)
{
	//Comprobaciones
  if(l==NULL)
  {										            //si la lista está vacía (l==NULL)
	   return NULL;                       //devolvemos NULL
  }// else Todo ha salido bien
  											          //si la lista no está vacía, devolver el campo cabecera de la lista
	return l->cabecera;
  
}





tipoPosicion siguiente(tipoPosicion p,Lista *l)    //devuelve "la posicion" siguiente a la posicion p de la lista l que se le pasa
{
  //Comprobaciones
  if (l==NULL || p==NULL)
  {
    return NULL;
  }//else Todo ha salido bien
  return p->sig;      //devolver el campo siguiente a p
                   
}





tipoPosicion anterior(tipoPosicion p, Lista *l)
{
    
    //Comprobación de parámetros
    if (p==NULL || l==NULL)
    {
      printf("Alguno de los parámetros es erróneo\n");
      return NULL;
    }
    
    //Declaraciones
    tipoPosicion auxiliar;                          //declaración de un auxiliar para usarlo de "guía" a través de la lista
    auxiliar = l->cabecera;                          //ponemos auxiliar al principio de la lista (para recorrerlaf)
    while (auxiliar->sig != p )                     //mientras el elemento siguiente a auxiliar sea diferente a p (el que buscamos)
    {
        auxiliar = auxiliar->sig;                   //pasamos al siguiente.
    }
    return auxiliar;                                //cuando aux->sig si que sea p, devolvemos auxiliar, ya que es la posición anterior a p
    
}





tipoPosicion fin(Lista *l)							//devuelve "la posicion" del ultimo elemento de la lista l que se le pasa
{
    //Comprobación de parámetros 
    if(l==NULL)
    {										//si la lista está vacía(l == NULL), devolver un NULL
        return NULL;
    }else
    {												//si la lista no está vacía, devolver "la posicion" del último elemento
        return l->ultimo;
    }
}



tipoPosicion localiza(tipoElemento x, Lista *l)
{
  //Comprobación de parámetros
  tipoElemento *x1 = &x;
  if (x1==NULL || l==NULL)
  {
    printf("Alguno de los parámetros es erróneo\n");
    return NULL;
  }
  
  //Declaraciones
  tipoPosicion auxiliar;
  auxiliar = l->cabecera;

  while (auxiliar->sig != NULL)
  {
    if (auxiliar->sig->elemento == x)
    {
      return auxiliar->sig;
    }else
    {
      auxiliar = auxiliar->sig;
    }

  }
  return NULL;

}



tipoElemento recupera(tipoPosicion p, Lista *l)
{
  //Comprobaciones de parámetros
  if (p==NULL || l==NULL)
  {
    printf("Alguno de los parámetros falla\n");
    return -1; //tipoElemento es entero, hay que devolver código de error entero
  }

  //Declaraciones
  tipoPosicion auxiliar;
  auxiliar = l->cabecera;

  while (auxiliar->sig != p)
  {
    auxiliar = auxiliar->sig;
  }
  return auxiliar->sig->elemento;


}


  //-----Funciones de modificación

int inserta(tipoElemento x, tipoPosicion p, Lista *l)
{
  //Declaraciones
  tipoPosicion auxiliar, aInsertar; //Declaramos dos variables de tipoPosicion, auxiliar, nos servirá para actualizar fondo, y aInsertar para añadir la nueva celda
  //Reserva de espacio
  aInsertar = (tipoPosicion)malloc(sizeof(tipoPosicion)); //Reservamos espacio para el elemento x
  //Comprobaciones
  if (p==NULL || l==NULL || aInsertar==NULL)
  {
    printf("Algunos valores fallan, o no se produjo la reserva de memoria\n");
    return -1;
  }
  
  //Intercambio y actualización de  (Dibujo en resúmenes)
  aInsertar->elemento = x; //Copiamos el elemento en aInsertar
  aInsertar->sig = p->sig;
  p->sig = aInsertar;
  //Actualización del fondo
  auxiliar=l->cabecera;         //situamos el auxiliar al inicio de la lista
  while (auxiliar->sig != NULL) //Nos vamos hasta el final de la lista
  {
    auxiliar = auxiliar->sig;
  }
  l->ultimo = auxiliar;  //actualizamos fondo, el último de la lista será auxiliar.
  return 0;              //porque hay que devolver un int, y con 0 indicamos que todo OK
}



int elimina(tipoPosicion p, Lista *l)									//funcion que elimina el elemento en la posición siguiente a p de la lista l
{
  //Comprobaciones
  if (p==NULL || l==NULL)
  {
    printf("Alguno de los parámetros es erróneo\n");
    return -1;//-1 indicará fallo en la función
  }
  //Declaración de vaiables
  tipoPosicion aBorrar, auxiliar;												//declaración de dos elementos de tipoPosicion

  //Intercambio de celdas y eliminación de aBorrar
  aBorrar = p->sig;													//el elemento a borrar será el apuntado por el campo siguiente de la pos p
  p->sig = aBorrar->sig;                    //Actualizamos valores
  free(aBorrar);														//liberamos el elemento a borrar
  
  //Actualización del fondo
  auxiliar=l->cabecera; 														
  while(auxiliar->sig!=NULL)
  {
    auxiliar=auxiliar->sig;
  }
  l->ultimo=auxiliar;

  return 0;
}


//-----Funciones auxiliares

tipoPosicion anula(Lista *l)							//funcion que anula la lista por completo, recorriendola y liberando.
{
  //Comprobación de la lista
  if(l->cabecera==NULL)
  {
        printf("\nLista vacia, no puede anularse\n\n");
        return l->cabecera; //Que es lo mismo que devolver NULL
  }
  if (l==NULL)
  {
    printf("La lista está a NULL\n");
    return NULL;
  }
  //Declaración de variables
  tipoPosicion aBorrar, auxiliar;

  auxiliar=l->cabecera;
  l->cabecera=NULL;
  while(auxiliar->sig !=NULL)
  {
	aBorrar = auxiliar;
	auxiliar = auxiliar->sig;
	free(aBorrar);
  }
	l->ultimo=NULL;
	printf("\nLista anulada correctamente\n\n");
	return l->cabecera;
  
}


void imprimeLista(Lista *l)
{

    //Comprobación de variables
    if(l->cabecera==NULL)
    {
        printf("\nLa lista está vacia \n");

    }
    if (l==NULL)
    {
      printf("La lista está a NULL\n");
    }
    //Declaración de variables
    tipoPosicion auxiliar;
    
    //Recorrido e impresión de la lista
    auxiliar=l->cabecera;                //Situamos el auxiliar al inicio de la lista
    
    printf("\n\tLista enlazada--> ");
    while(auxiliar->sig !=NULL)
    {   
        printf("%d ",auxiliar->elemento);
        auxiliar=auxiliar->sig;
    }
    
    printf("\n\n");
}
