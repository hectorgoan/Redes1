#ifndef __LISTA_H
#define __LISTA_H

typedef int tipoElemento;       

typedef struct tipoCelda        
{
	tipoElemento elemento;        
	struct tipoCelda * sig;       
} tipoCelda;                    


typedef struct                  
{
	tipoCelda * cabecera;         
  	tipoCelda * ultimo;           
} Lista;                        

typedef tipoCelda *tipoPosicion;    

void iniciarLista(Lista *l);
tipoPosicion primero(Lista *l);
tipoPosicion siguiente(tipoPosicion p,Lista *l);  
tipoPosicion anterior(tipoPosicion p, Lista *l);
tipoPosicion fin(Lista *l);
tipoPosicion localiza (tipoElemento x, Lista *l);
tipoElemento recupera(tipoPosicion p, Lista *l);
int inserta(tipoElemento x, tipoPosicion p, Lista *l);
int elimina(tipoPosicion p, Lista *l);
tipoPosicion anula(Lista *l);
void imprimeLista(Lista *l);

#endif