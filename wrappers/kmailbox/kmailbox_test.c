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
    while (1) sleep(1);

  mysend(child, strlen(str), str);


  return 0;
}
