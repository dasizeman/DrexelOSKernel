#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "kmailbox_usr.h"

int main(int argc, char** argv)
{
  char *str = (char*)malloc(100);
  strcpy(str, "This is a test message!");

  pid_t child;
  pid_t parent = getpid();

  if ((child = fork()) == 0)
  {
    char *recvstring = (char*)malloc(100);
    int res = 0;
    while (res <= 0)
    {
      res = myreceive(parent, strlen(str), recvstring);
      if (res > 0)
        printf("%s\n", recvstring);
    }
    return 0;
  }
  else
  {
    mysend(child, strlen(str), str);
  }

  return 0;
}
