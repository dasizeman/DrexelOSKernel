#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <strings.h>

int main()
{
  // Create a shared memory space
  key_t key;
  pid_t *pids;
  int i;
  key = ftok("./370_wa1.c", 'a');
  if (key == -1) perror("ftok");

  i = shmget(key, 12, 0666 | IPC_CREAT); // 12 bytes = 3 pids
  if (i<0) perror("shmget");
  pids = shmat(i, NULL, 0);
  if (pids == -1) perror("shmat");
  bzero(pids,12);

  pids[0] = getpid();
  pid_t temp;

  if ((temp = fork()) == 0) // Son
  {
    if ((temp = fork()) == 0) // Grandson
    {
      printf("My pid is %d, my parent's pid is %d, and my grandparent's pid is %d!\n",
          pids[2], pids[1], pids[0]);
    }
    else
      pids[2] = temp;
  }
  else
    pids[1] = temp;


  shmdt(pids);

  return 0;
}
