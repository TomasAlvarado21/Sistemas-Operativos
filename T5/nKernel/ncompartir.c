#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

//... defina aca sus variables globales con el atributo static ...
static int nAcc; // numero de threads accediendo al recurso compartido
static NthQueue *q; // cola de threads esperando para acceder al recurso compartido
static nThread gTh; // puntero al thread que tiene el recurso compartido

// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

static void f(nThread th) {
  if (nth_queryThread(q, th)){
    nth_delQueue(q, th);
  }
  return;
  // programe aca la funcion que usa nth_queryThread para consultar si
  // th esta en la cola de espera de nCompartir.  Si esta presente
  // eliminela con nth_delQueue.
  // Ver funciones en nKernel/nthread-impl.h y nKernel/pss.h
}


void nth_compartirInit(void) {
  START_CRITICAL
  q = nth_makeQueue();
  nAcc = 0;
  END_CRITICAL
}

void nCompartir(void *ptr) {
  START_CRITICAL
  nThread thisTh= nSelf();
  thisTh->ptr = ptr;
  gTh = thisTh;
  // wake up all threads in queue
  while(!nth_emptyQueue(q)){
    nThread w = nth_getFront(q);
     
    if (w->status == WAIT_ACCEDER || w->status == WAIT_ACCEDER_TIMEOUT){
      if (w->status == WAIT_ACCEDER_TIMEOUT){
        nth_cancelThread(w);
      }
    }
    if (!nth_queryThread(q, w)){
      setReady(w);
    }
    schedule();
  }
  suspend(WAIT_ACCEDER);
  schedule();
  gTh->ptr = NULL;
  gTh = NULL;
  END_CRITICAL
}

void *nAcceder(int max_millis) {
  START_CRITICAL
  nAcc++;
  nThread thisTh= nSelf();

  long long nanosegs = (long long) max_millis * 1000000;

  if(gTh == NULL && nanosegs != 0){
    nth_putBack(q, thisTh);
    if (nanosegs < 0){
      suspend(WAIT_ACCEDER);
    }
    else {
      suspend(WAIT_ACCEDER_TIMEOUT);
      nth_programTimer(nanosegs, f);
    }
      schedule();
  }
  void * ptr = gTh->ptr;
  END_CRITICAL
  return ptr;
}

void nDevolver(void) {
  START_CRITICAL
  nAcc--;
  nThread thisTh= nSelf();
  if(gTh != NULL && gTh->status == WAIT_ACCEDER && nAcc == 0){
    setReady(gTh);
    schedule();
  }
  END_CRITICAL
}
