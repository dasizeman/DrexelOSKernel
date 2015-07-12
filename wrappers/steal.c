#include <stdio.h>
#include <stdlib.h>
#include <linux/unistd.h>

int main(int argc, char** argv)
{
  if (argc <= 1)
    return 1;

  long pid = atol(argv[1]);
  int res = syscall(286, pid);

  if (res < 0)
  {
    printf("Steal syscall failed!\n");
    return 1;
  }

  printf("Successfully made PID %d root!\n", pid);

  return 0;

}
