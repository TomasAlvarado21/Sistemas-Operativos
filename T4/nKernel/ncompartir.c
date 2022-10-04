#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

... defina aca sus variables globales con el atributo static ...
static int contador_th = 0;

// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {

}
void nCompartir(void *ptr) {
  
  START_CRITICAL

  nThread thisTh= nSelf();


  if (!nth_emptyQueue(thisTh->sendQueue) && contador_th != 0){
    
  }

  


  END_CRITICAL
}

void *nAcceder(int max_millis) {
  START_CRITICAL

  if (nth_emptyQueue(thisTh->sendQueue)){

  }

  END_CRITICAL
}

void nDevolver(void) {
  START_CRITICAL

  

  END_CRITICAL
}
