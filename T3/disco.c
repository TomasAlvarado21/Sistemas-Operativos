#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "pss.h"
#include "disco.h"

// Defina aca sus variables globales
typedef struct {
  char *nombre;
  int ready;
  pthread_cond_t w;
} Request;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

int busyV = 0;
int busyD = 0;

Queue *Varones;
Queue *Damas;


void DiscoInit(void) {
  // ... inicialice aca las variables globales ...
}

void DiscoDestroy(void) {
  // ... destruya las colas para liberar la memoria requerida ...
}

char *dama(char *nom) {
  // ...
}

char *varon(char *nom) {
  pthread_mutex_lock(&mutex);
  if (!emptyQueue(Damas)) {
    Request *nombre = get(Damas);
    pthread_cond_signal(&nombre->w);
    return dama;
  }
  
  
}
