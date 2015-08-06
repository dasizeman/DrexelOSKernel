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

  if ((child = fork()) == 0)
  {
    char *recvstring = (char*)malloc(100);
    int res;
    res = myreceive(-1, strlen(str), recvstring);
    //printf("%s\n", recvstring);
    printf("Tried myreceive, got %d\n", res);
  }
  else
  {
    mysend(child, strlen(str), str);
  }

  return 0;
}
