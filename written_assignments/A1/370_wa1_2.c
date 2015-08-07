#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <strings.h>

int caught_signal = 0;

void handler (int arg)
{
  printf("Parent: signal received!\n");
  caught_signal = 1;
}


int main()
{
  // Create a shared memory space
  key_t key;
  pid_t *pids;
  int i;
  key = ftok("./370_wa1_2.c", 'a');
  if (key == -1) perror("ftok");

  i = shmget(key, 12, 0666 | IPC_CREAT); // 12 bytes = 3 pids
  if (i<0) perror("shmget");
  pids = shmat(i, NULL, 0);
  if (pids == -1) perror("shmat");
  bzero(pids,12);

  pids[0] = getpid();
  printf("getpid() : %d\n", getpid());
  printf("pids[0] : %d\n", pids[0]);
  pid_t temp;

  // Set up the signal handler
  if (signal(SIGUSR1, handler) == SIG_ERR)
    printf("Problem attaching signal handler!\n");

  pid_t child2, child3;

  // Create the first child
  if (fork() == 0)
  {
    printf("First child: sending signal to the parent\n");
    kill(pids[0], SIGUSR1);
  }

  if (getpid() == pids[0]) // Only the parent
    if ((child2 = fork()) == 0) // Second child
    {
      printf("Second child: going to sleep...\n");
      sleep(10);
      printf("Second child: Exiting!\n");
      return 0;
    }

  if (getpid() == pids[0])
  {
    if ((child3 = fork()) == 0) // Third child
    {
      printf("Third child: Will now wait...\n");
      int status;
      wait(&status);
    }  
    else
    {
      printf("Parent: Waiting for signal...\n");
      while (caught_signal == 0) sleep(1);
      printf("Parent: Signal received! Waiting for second child...\n");
      int status;
      waitpid(child2, &status, 0);
      printf("Parent: Second child dead! Killing third child and exiting.\n");
      kill(child3,SIGKILL);
    }

    shmdt(pids);
  }


  return 0;
}
