#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "linked_list.h"
#include "disastrOS_globals.h"



void internal_semOpen(){
  // do stuff :)
  int semnum=running->syscall_args[0];

  int value=running->syscall_args[1];

  //we have to check if the semaphore is created in the system
  ListHead semaphores_used = semaphores_list;

  Semaphore* sem=SemaphoreList_byId(&semaphores_used,semnum);

  if(!sem){
    //if it doesn't exist in the system we have to create it
    Semaphore_init();
    sem = semaphore_alloc(semnum, value);
    //now we have to add th esemaphore to the semaphore list of the system
    List_insert(&semaphores_list, semaphore_list.last, (ListItem*) sem);
    int fd = running->last_sem_fd + 1;
    //we have to create the semdescriptor
    SemDescriptor_init();
    SemDescriptor sem_desc = SemDescriptor_alloc(fd, sem, &running);

    running->syscall_retvalue = semnum;

  }
  else{
    //if it exist in the system
    //we have to check that the semaphore isn't already open in the process
    ListHead semaphores_opened = running->sem_descriptors;
    Semaphore* opened = Search_id(&semaphores_opened, semnum);
    if(opened){
      running->syscall_retvalue = semnum;
    }
    else{
      int fd = running->last_sem_fd + 1;
      //we have to create the semdescriptor
      SemDescriptor_init();
      SemDescriptor sem_desc = SemDescriptor_alloc(fd, sem, &running);
      running->syscall_retvalue = semnum;
    }

  }
}
