#include <stdio.h>
#include <stdlib.h>
#include <linux/unistd.h>

int main(int argc, char** argv)
{

  if (argc <= 1)
    return 1;

  long pid = atol(argv[1]);
  unsigned int res = syscall(287, pid);

  if (res > 0)
  {
    printf("PID %d timeslice is now %d!\n", pid, res);
    return 0;
  }

  printf("quad syscall failed!\n");
  return 1;
}
