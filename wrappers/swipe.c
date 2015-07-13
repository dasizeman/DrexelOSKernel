#include <stdio.h>
#include <stdlib.h>
#include <linux/unistd.h>

int main(int argc, char** argv)
{
  if (argc < 3)
    return 1;

  long target, victim;
  target = atol(argv[1]);
  victim = atol(argv[2]);

  unsigned int res = syscall(288, target, victim);

  if (res == 0)
  {
    printf("swipe syscall failed or no time stolen!\n");
    return 1;
  }

  printf("Stole %d ns from victim %d and gave them to target %d!\n",
      res, victim, target);

  return 0;
}
