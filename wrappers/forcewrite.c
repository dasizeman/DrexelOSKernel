#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
  if (argc < 2)
    return 1;

  int file;

  if ((file = open(argv[1], O_RDONLY|O_APPEND)) < 0)
  {
    printf("Could not open file specified on command line.\n");
    return 1;
  }

  printf("got fd %d\n", file);

  char* buff = (char*)malloc(500);
  strcpy(buff, "You have been force written!\n");

  if (syscall(291, file, buff, strlen(buff)) < 0)
  {
    printf("Error in write call!\n");
    return 1;
  }

  printf("Successfully appended to %s!\n", argv[1]);

  return 0;
}
