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
  int fd = running->last_sem_fd;

  if(!sem){
    //if it doesn't exist in the system we have to create it
    sem = Semaphore_alloc(semnum, value);
    printf("lo alloco con sem_num=%d\n",semnum);

    if(!sem){
      running->syscall_retvalue = DSOS_ECREATESEMAPHORE; //the semaphore couldn't be create
      return;
    }
    //now we have to add the semaphore to the semaphore list of the system
    List_insert(&semaphores_list, semaphores_list.last, (ListItem*) sem);

  }

  //we have to check that the semaphore isn't already open in the process
  ListHead semaphores_opened = running->sem_descriptors;

  SemDescriptor* opened = Search_id(&semaphores_opened, semnum);
  if(opened){
    running->syscall_retvalue = opened->fd;
    return;
  }
  else{
    (running->last_sem_fd)++;
    //we have to create the semdescriptor
    SemDescriptor* sem_desc = SemDescriptor_alloc(fd, sem, running);
    if(!sem_desc){
      running->syscall_retvalue = DSOS_ECREATESEMDESC; //the semDESCRIPTOR couldn't be create
      return;
    }

    SemDescriptorPtr * sem_desc_ptr = SemDescriptorPtr_alloc(sem_desc);
    if(!sem_desc_ptr) {
      running->syscall_retvalue = DSOS_ECREATESEMDESCPTR; //the semDESCRIPTORpointer couldn't be create
      return;
    }
    sem_desc->ptr = sem_desc_ptr;

    SemDescriptorPtr * sem_desc_ptr_waiter = SemDescriptorPtr_alloc(sem_desc);
    if(!sem_desc_ptr_waiter) {
      running->syscall_retvalue = DSOS_ECREATESEMDESCPTR; //the semDESCRIPTORpointer couldn't be create
      return;
    }
    sem_desc->ptr_waiter = sem_desc_ptr_waiter;

    //we add teh descriptor pointer to the list
    List_insert(&running-> sem_descriptors,running->sem_descriptors.last,(ListItem*) sem_desc);
    List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*)sem_desc_ptr);
    running->syscall_retvalue = fd;
    }


}
