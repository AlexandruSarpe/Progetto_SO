#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  // do stuff :)
  // Prendo argomento numero del semaforo
  int semNumber=running->syscall_args[0];

  // Prendo la lista dei semafori attivi per quel processo
  ListHead sems=running->sem_descriptors;

  // Controllo se il semaforo esiste ed è nella lista
  Semaphore* semaphore=Search_id(&sems, semNumber);

  // se non c'è;
  if (!sem) running->syscall_retvalue=DSOS_ESEMNOTOPENED;

}
