#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "bolsa.h"


pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond= PTHREAD_COND_INITIALIZER;

int mejor_precio = 0;

int *state;

char **comprador_actual;

char vendedor_actual[100];


int vendo(int precio, char *vendedor, char *comprador) {
  int estado_local = 0;

  pthread_mutex_lock(&m);
  
  if (mejor_precio >= precio || mejor_precio == 0){
    mejor_precio = precio;
    if (state != NULL){
      *state = -1;
    }
    state = &estado_local;
    
    strcpy(vendedor_actual, vendedor);
    comprador_actual = &comprador;

    pthread_cond_broadcast(&cond);

    while (!estado_local){
      pthread_cond_wait(&cond, &m);
    }

    pthread_mutex_unlock(&m);
    if (estado_local == 1){
      return 1;
    }
    else{
      return 0;
    }
    
  }
  estado_local = -1;

  pthread_mutex_unlock(&m);

  return 0;

}

int compro(char *comprador, char *vendedor) {

  int precio = 0;

  pthread_mutex_lock(&m);

  precio = mejor_precio;

  if (precio != 0){    

    *state = 1;
    state = NULL;

    strcpy(vendedor, vendedor_actual); 
    
    strcpy(*comprador_actual, comprador);
    
    strcpy(vendedor_actual, ""); 

    comprador_actual = NULL;


    mejor_precio = 0;

    pthread_cond_broadcast(&cond);


    pthread_mutex_unlock(&m);

    return precio;
  }
  
  pthread_mutex_unlock(&m);

  return precio;

}
