#include <stdio.h>
#include <stdlib.h>
#include <linux/unistd.h>

int main(int argc, char** argv)
{
  if (argc <= 1)
    return 1;

  long pid = atol(argv[1]);
  syscall(286, pid);

}
