#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "disk.h"
#include "priqueue.h"
#include "spinlocks.h"

// Le sera de ayuda la clase sobre semáforos:
// https://www.u-cursos.cl/ingenieria/2022/2/CC4302/1/novedades/detalle?id=431689
// Le serviran la solucion del productor/consumidor resuelto con el patron
// request y la solucion de los lectores/escritores, tambien resuelto con
// el patron request.  Puede substituir los semaforos de esas soluciones
// por spin-locks, porque esos semaforos almacenan a lo mas una sola ficha.

typedef struct{
  int t;
  int w;
}Request;

// Declare los tipos que necesite

PriQueue *higher_track; // priority queue con los request mayores o ituales a t
PriQueue *lower_track; // priority queue con los request menores a t
int sl; // spinlock que controla el acceso a la zona critica
int last_track; // variable global que guarda la ultima pista t 

// Declare aca las variables globales que necesite
// ...

// Agregue aca las funciones requestDisk y releaseDisk

void iniDisk(void) {
  // inicializamos las variables
  sl = OPEN;
  last_track = -1;
  higher_track = makePriQueue();
  lower_track = makePriQueue();
}

void requestDisk(int track) {
  spinLock(&sl);

  if (last_track == -1){
    last_track = track;
    spinUnlock(&sl);
    return;
  }else{
    // inicializamos un spinlock cerrado para este track, dejandolo en espera
    int w = CLOSED;
    Request req = {track, w};
    // como el disco esta ocupado, tenemos que dejar este track en espera ademas de colocarlo en la cola de prioridad 
    if (track >= last_track){ // vemos si el track es mayor o igual al ultimo track, si es asi lo colocamos en el PriQueue higher sino en el lower
      priPut(higher_track, &req, track);
    }else{
      priPut(lower_track, &req, track);
    }
    spinUnlock(&sl); // hacemos spinUnlock para poder hacer lock a w
    spinLock(&w); // dejamos el w en espera de su turno
    return;
  }
}

void releaseDisk() {
  spinLock(&sl); // garantizamos la exclusion mutua

  if (emptyPriQueue(higher_track) && emptyPriQueue(lower_track)){
    last_track = -1;
  }else if(!emptyPriQueue(higher_track)){
    Request *puntero = priGet(higher_track);
    spinUnlock(&(puntero->w));
  }else{

    while (!emptyPriQueue(lower_track)){
      Request *get = priGet(lower_track);
      priPut(higher_track, &(get->w), get->t);
    }
    
    Request *puntero = priGet(higher_track);
    spinUnlock(&(puntero->w));
  }
  spinUnlock(&sl);
}
