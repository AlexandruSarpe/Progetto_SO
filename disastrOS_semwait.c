#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  // do stuff :)
  int fd=running->syscall_args[0];


  ListHead process_semaphores = running->sem_descriptors;

  //we have to check if the semaphore is opened in the process
  SemDescriptor* sd = SemDescriptorList_byFd(&process_semaphores, fd);

  Semaphore* sem=sd->semaphore;

  if(!sem){
    running->syscall_retvalue = DSOS_ESEMNOTOPENED;
  }

  else if(sem->count<=0){
    //the process has to wait in the waiting queue
    List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last, (ListItem*)running);
    //we have to change the state of the process_semaphores
    running->status = Waiting;
    //and we insert the process in the waiting list
    List_insert(&waiting_list, waiting_list.last, (ListItem*)running);
    //if there is a process in wating list of the semaphore we have to awake him, else we have to set speeper as run prcess
    if (ready_list.first) {
      running=(PCB*) List_detach(&ready_list, ready_list.first);
      running->syscall_retvalue = 0;
  } else {
      running=0;
      printf ("they are all sleeping\n");
      running->syscall_retvalue = 0;
    }
  }
  else{
    (sem->count)--;
    running->syscall_retvalue = 0;
  }
}
