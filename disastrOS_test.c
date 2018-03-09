#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "disastrOS.h"


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
  printf("I will iterate a bit, before terminating\n");
  int type=0;
  int mode=0;
  int fd=disastrOS_openResource(disastrOS_getpid(),type,mode);
  printf("fd=%d\n", fd);
  printf("PID: %d, terminating\n", disastrOS_getpid());

  for (int i=0; i<(disastrOS_getpid()+1); ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);
  }
  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  /*disastrOS_printStatus();
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






  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }*/
  //in this function we have to test our semaphores_list
  int fd_1, fd_2, fd_3, ret;

  int semnum_1 = 1;
  int semnum_2 = 2;
  int semnum_3 = 3;
  //we open the semaphore
  fd_1 = DisastrOS_semOpen(semnum_1, 1);
  fd_2 = DisastrOS_semOpen(semnum_2, 1);
  fd_3 = DisastrOS_semOpen(semnum_3, 1);


  printf("the semaphores have been opened, with fd1=%d, fd2=%d, and fd3=%d\n",fd_1,fd_2, fd_3);

  ret = DisastrOS_semClose(fd_1);
  printf("the semaphore has been closed with ret= %d\n",ret);

  ret = DisastrOS_semClose(fd_2);

  printf("the semaphore has been closed with ret= %d\n",ret);

  ret = DisastrOS_semClose(fd_3);

  printf("the semaphore has been closed with ret= %d\n",ret);



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
