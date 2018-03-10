#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "disastrOS.h"
#include "disastrOS_globals.h"


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

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  int sem= DisastrOS_semOpen(1, 1);
  ERROR_HELPER(sem < 0,"Error semOpen sem_fill process ");
  disastrOS_printStatus();
  int ret = DisastrOS_semWait(sem);
  ERROR_HELPER(ret != 0, "Error semWait sem process ");
  printf("I'm in cs pid=%d",running->pid);
  ret = DisastrOS_semPost(sem);
  ERROR_HELPER(ret != 0, "Error semPost sem process ");
  ret = DisastrOS_semClose(sem);
  ERROR_HELPER(ret != 0, "Error semClose sem process ");
  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);


  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;
  for (int i=0; i<10; ++i) {
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    int fd=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }
  printf("shutdown!");
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
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
