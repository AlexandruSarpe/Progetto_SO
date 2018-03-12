
#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "disastrOS.h"
#include "disastrOS_globals.h"

#define SEM_FILL 0
#define SEM_EMPTY 1
#define SEM_MUTEX1 2
#define SEM_MUTEX2 3
#define BUFFER_LENGTH_SEM 5
#define ITERATIONS 10
#define HOWMANY 10

#define ERROR_HELPER(cond, msg) do {    \
        if (cond && (running->pid!=0)) {              \
            printf("%s: %d \n", msg,running->pid);      \
              disastrOS_exit(disastrOS_getpid()+1); \
        }   \
        else if (cond && (running->pid==0)){     \
            printf("%s:",msg);  \
            disastrOS_exit(disastrOS_getpid()+1); \
        }   \
    } while(0)  \


void initFunction_semaphores(void* args);
void PrintBuffer(int * buffer);
void Prod(void* args);
void Cons(void* args);
// we need this to handle the sleep state

void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}


void Prod(void* args){
    int i,ret;

    printf("Starting producer with pid : %d\n",running->pid);
    //opening the semaphores
    int sem_fill= DisastrOS_semOpen(SEM_FILL, 0);
    ERROR_HELPER(sem_fill < 0,"Error semOpen sem_fill process ");

    int sem_empty = DisastrOS_semOpen(SEM_EMPTY, BUFFER_LENGTH_SEM);
    ERROR_HELPER(sem_empty < 0,"Error semOpen sem_empty process ");

    int sem_mutex1 = DisastrOS_semOpen(SEM_MUTEX1, 1);
    ERROR_HELPER(sem_mutex1 < 0,"Error semOpen sem_mutex1 process ");


    for(i = 0;i < ITERATIONS;i++){
        ret = DisastrOS_semWait(sem_empty);
        ERROR_HELPER(ret != 0, "Error semWait sem_empty process ");
        ret = DisastrOS_semWait(sem_mutex1);
        ERROR_HELPER(ret != 0, "Error semWait sem_mutex1 process ");

        printf("Hello, i am prod and i am in CS! Pid : %d\n",running->pid);


        ret = DisastrOS_semPost(sem_mutex1);
        ERROR_HELPER(ret != 0, "Error semPost sem_mutex1 process ");


        ret = DisastrOS_semPost(sem_fill);
        ERROR_HELPER(ret != 0, "Error semPost sem_fill process ");
    }


    ret = DisastrOS_semClose(sem_fill);
    ERROR_HELPER(ret != 0, "Error semClose sem_fill process");

    ret = DisastrOS_semClose(sem_empty);
    ERROR_HELPER(ret != 0, "Error semClose sem_empty process");

    ret = DisastrOS_semClose(sem_mutex1);
    ERROR_HELPER(ret != 0, "Error semClose sem_mutex1 process");


    disastrOS_exit(disastrOS_getpid()+1);
}

void Cons(void* args){
    int i,ret;

    printf("Starting consumer with pid : %d\n",running->pid);

    //opening the semaphores
    int sem_fill= DisastrOS_semOpen(SEM_FILL, 0);
    ERROR_HELPER(sem_fill < 0,"Error semOpen sem_fill process ");

    int sem_empty = DisastrOS_semOpen(SEM_EMPTY, BUFFER_LENGTH_SEM);
    ERROR_HELPER(sem_empty < 0,"Error semOpen sem_empty process ");

    int sem_mutex2 = DisastrOS_semOpen(SEM_MUTEX2, 1);
    ERROR_HELPER( sem_mutex2 < 0,"Error semOpen sem_mutex2 process ");


    for(i = 0;i < ITERATIONS;i++){

        ret = DisastrOS_semWait(sem_fill);
        ERROR_HELPER(ret != 0, "Error semWait sem_fill process");

        ret = DisastrOS_semWait(sem_mutex2);
        ERROR_HELPER(ret != 0, "Error semWait sem_mutex2 process ");

        printf("Hello,i am the cons and i am in CS! Pid : %d\n",running->pid);

        ret = DisastrOS_semPost(sem_mutex2);
        ERROR_HELPER(ret != 0, "Error semPost sem_mutex2 process ");

        ret = DisastrOS_semPost(sem_empty);
        ERROR_HELPER(ret != 0, "Error semPost sem_empty process ");

    }

    ret = DisastrOS_semClose(sem_fill);
    ERROR_HELPER(ret != 0, "Error semClose fd_fill process ");
    ret = DisastrOS_semClose(sem_empty);
    ERROR_HELPER(ret != 0, "Error semClose fd_empty process ");
    ret = DisastrOS_semClose(sem_mutex2);
    ERROR_HELPER(ret != 0, "Error semClose fd_me2 process ");

    disastrOS_exit(disastrOS_getpid()+1);
}



void initFunction_semaphores(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started pid=%d\n",running->pid);
  disastrOS_spawn(sleeperFunction, 0);


  printf("I feel like to spawn 10 nice processes\n");
  int children=0;
  int i;
  int fd[10];
  for (i=0; i<5; ++i) {
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource\n");
    fd[i]=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd[i]);
    disastrOS_spawn(Prod, 0);
    children++;
  }

  for (; i<10; ++i) {
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource\n");
    fd[i]=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd[i]);
    disastrOS_spawn(Cons, 0);
    children++;
  }
  int retval;
  int pid;
  while(children>0 && (pid=disastrOS_wait(0, &retval))>=0){

    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
     pid, retval, children);
    --children;
  }
  for (i=0; i<10; ++i) {
    printf("closing resource %d\n",fd[i]);
    disastrOS_closeResource(fd[i]);
    disastrOS_destroyResource(i);
  }

  disastrOS_printStatus();

  printf("shutdown!\n");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  // spawn an init process

  printf("start\n");
  disastrOS_start(initFunction_semaphores, 0, logfilename);
  return 0;
}
