#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "kmailbox_usr.h"

void send_and_wait(pid_t,int);

int main(int argc, char** argv)
{
  
  int n=5;
  pid_t parent = getpid();

  pid_t child_pids[n];

  int i;

  // Make n children
  for (i = 0; i<n; i++)
  {
    if (getpid() != parent) // Only the parent should spawn new children
    break;

    pid_t this_child = fork();
    if (this_child == 0)
    {
      send_and_wait(parent,i);
      return 0;
    }
    else
      child_pids[i] = this_child;
  }

  if (getpid() == parent)
  {
    char message[] = "Beep!";
    char recvmessage[6];
    int count = 1;

    while (count <= n)
    {
      if (myreceive(-1, 6,recvmessage) > 0)
      {
        printf("Parent: received %d message(s).\n", count);
        count++;
      }
    }
    
    for (i=0; i < n; i++)
    {
      printf("Parent: sending message to child %d.\n", i);
      mysend(child_pids[i], 6,message);
    }

  }


  return 0;
}

void send_and_wait(pid_t to, int n)
{
  char message[] = "Boop!";
  mysend(to, 6,message);
  int res = 0;

  printf("Child %d: sent message to parent and awating response\n", n);

  while (res <= 0)
  {
    res = myreceive(-1, 6,message);
  }

  printf("Child %d: got message from parent, will now exit.\n", n);
}
