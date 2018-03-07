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
    SemDescriptor* sem_d=SemDescriptorList_byFd(&sems,//TODO);

    // se non c'è;
    if (!sem_d) {
        running->syscall_retvalue=DSOS_ERESOURCENOFD;
        return;
    }

    // prendo il valore attuale del semaforo e lo aumento di 1
    int count=(sem_d->semaphore->count)++;

    // prendo la lista dei processi in waiting che usano quel semaforo
    ListHead w_desc=sem_d->semaphore->waiting_descriptors;

    if (count>0) {
        // rimuovo il processo dalla waiting queue
        List_detach(&waiting_list,(ListItem*)running);

        // inserisco il processo nella ready queue
        List_insert(&ready_list,ready_list.last,(ListItem*)running);

        // rimuovo il processo dai waiting descriptors del semaforo
        List_detach(&w_desc,(ListItem*)running);

        //TODO
    }

    running->syscall_retvalue = 0;;
}
