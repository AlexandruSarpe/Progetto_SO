#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  // do stuff :)
  int semnum=running->syscall_args[0];


  ListHead process_semaphores = running->sem_descriptors;

  //we have to check if the semaphore is opened in the process
  Semaphore* sem = Search_id(&process_semaphores, semnum);

  if(!sem){
    running->syscall_retvalue = DSOS_ESEMNOTOPENED;
  }

  else if(sem->count<=0){
    //the process has to wait in the waiting queue
    List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last, (ListItem*)&running);
    //we have to change the state of the process_semaphores
    running->status = Waiting;
    //and we insert the process in the waiting list
    List_insert(&waiting_list, waiting_list.last, (ListItem*)&running);
    running->syscall_retvalue = 0;
  }
  else{
    sem->count --;
    running->syscall_retvalue = 0;
  }


}
