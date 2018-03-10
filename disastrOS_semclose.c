#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "pool_allocator.h"

void internal_semClose(){
  // do stuff :)
  int sem_fd=running->syscall_args[0];

  int ret;

  // prendo il SemDescriptor
  SemDescriptor* sem_d=SemDescriptorList_byFd(&(running->sem_descriptors),sem_fd);

  if (!sem_d) {
      running->syscall_retvalue=DSOS_ESEMNOTOWNED;
      return;
  }

  // prendo il semaforo all'interno
  Semaphore* s=sem_d->semaphore;

  // tolgo me stesso dai descriptors del semaforo
  List_detach(&(s->descriptors),(ListItem*)sem_d->ptr);

  if (!(s->descriptors).size) {
     s = (Semaphore*) List_detach(&semaphores_list, (ListItem*) s);
     ret=Semaphore_free(s);
     if (ret!=0x0) {
        // printf("Errore Semaphore_free: %s\n",PoolAllocator_strerror((PoolAllocatorResult) ret));
         running->syscall_retvalue=DSOS_ESEMFREE;
         return;
     }
   }
   SemDescriptorPtr* sem_d_ptr = sem_d->ptr;
   SemDescriptorPtr* sem_d_ptr_waiter = sem_d->ptr_waiter;
   ret=SemDescriptor_free(sem_d);
   if (ret!=0x0) {
    // printf("Errore Semaphore_free: %s\n",PoolAllocator_strerror((PoolAllocatorResult) ret));
    running->syscall_retvalue= DSOS_ESEMDESCFREE;
    return;
   }
   //we have to free also the semdescriptor pointer
   ret=SemDescriptorPtr_free(sem_d_ptr);
   if (ret!=0x0) {
    // printf("Errore Semaphore_free: %s\n",PoolAllocator_strerror((PoolAllocatorResult) ret));
    running->syscall_retvalue= DSOS_ESEMDESCPTRFREE;
    return;
   }
   ret=SemDescriptorPtr_free(sem_d_ptr_waiter);
   if (ret!=0x0) {
    // printf("Errore Semaphore_free: %s\n",PoolAllocator_strerror((PoolAllocatorResult) ret));
    running->syscall_retvalue= DSOS_ESEMDESCPTRFREE;
    return;
   }
   running->syscall_retvalue=ret;
   return;
}
