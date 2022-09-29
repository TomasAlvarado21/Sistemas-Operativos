#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "pss.h"
#include "disco.h"

// Defina aca sus variables globales
typedef struct {
  char *nombre_varon;
  char *nombre_dama;
  int ready;
  pthread_cond_t w;
} Request;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;


Queue *Varones;
Queue *Damas;


void DiscoInit(void) {
  // ... inicialice aca las variables globales ...
  Varones = makeQueue();
  Damas = makeQueue();
}

void DiscoDestroy(void) {
  // ... destruya las colas para liberar la memoria requerida ...
  destroyQueue(Varones);
  destroyQueue(Damas);
}

char *dama(char *nom) {
  char *local_varon = NULL;
  pthread_mutex_lock(&m);
  if (!emptyQueue(Damas) || emptyQueue(Varones)){
    Request req_dama = {"", nom , 0, PTHREAD_COND_INITIALIZER};
    put(Damas, &req_dama);
    while (!req_dama.ready){
      pthread_cond_wait(&req_dama.w, &m);
      local_varon = req_dama.nombre_varon;
    }
  }
  else {
    Request *req_varon = get(Varones);
    req_varon->ready = 1;
    local_varon = req_varon->nombre_varon; 
    req_varon->nombre_dama=nom;
    pthread_cond_signal(&req_varon->w);
  }
  pthread_mutex_unlock(&m);
  return local_varon;
}

char *varon(char *nom) {
  char *local_dama = NULL;
  pthread_mutex_lock(&m);
  if(emptyQueue(Damas) || !emptyQueue(Varones)){
    Request req_varon = {nom, "", 0, PTHREAD_COND_INITIALIZER};
    put(Varones, &req_varon);
    while (!req_varon.ready){
      pthread_cond_wait(&req_varon.w, &m);
      local_dama = req_varon.nombre_dama;
    }
  }
  else {
    Request *req_dama = get(Damas);
    req_dama->ready = 1;
    req_dama->nombre_varon = nom;
    local_dama = req_dama->nombre_dama;
    pthread_cond_signal(&req_dama->w);
  }
  pthread_mutex_unlock(&m);
  return local_dama;
}
