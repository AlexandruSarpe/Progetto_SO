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
    int sem_fd=running->syscall_args[0];

    // Prendo la lista dei semafori attivi per quel processo
    ListHead sems=running->sem_descriptors;

    // Controllo se il descrittore del semaforo esiste ed è nella lista
    SemDescriptor* sem_d=SemDescriptorList_byFd(&sems,sem_fd);

    // se non c'è;
    if (!sem_d) {
        running->syscall_retvalue=DSOS_ESEMNOTOWNED;
        return;
    }

    // se c'è prendo il semaforo
    Semaphore* s=sem_d->semaphore;

    if (!s) {
        running->syscall_retvalue=DSOS_ESEMNOTOPENED;
        return;
    }

    if (s->count<0) {
        // rimuovo il primo processo in cima alla waiting queue del semaforo
        SemDescriptorPtr* toReady_proc_waiter=(SemDescriptorPtr*)List_detach(&(s->waiting_descriptors),(ListItem*)s->waiting_descriptors.first);

        // prendo il PCB di quel processo
        PCB* toReady_proc_waiter_pcb=toReady_proc_waiter->descriptor->pcb;

        // rimuovo il processo dalla waiting list del sistema
        List_detach(&waiting_list,(ListItem*)toReady_proc_waiter_pcb);

        // metto lo status del processo rimosso dalla waiting list a Ready
        toReady_proc_waiter_pcb->status=Ready;

        // inserisco il processo nella ready list del sistema
        List_insert(&ready_list,ready_list.last,(ListItem*)toReady_proc_waiter_pcb);
    }

    // aumento valore del semaforo
    (s->count)++;

    //setto il valore di ritorno a 0 per indicare successo
    running->syscall_retvalue = 0;

}
