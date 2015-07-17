#include <stdio.h>
#include <time.h>
#include <linux/unistd.h>

void loop_work(int seconds);

int main(int argc, char** argv)
{
  printf("Forking a child...\n");
  pid_t child = fork();

  if (child == 0)
    loop_work(15);

  if (child != 0)
  {
    printf("Waiting until my child %d finishes!\n", child);
    syscall(290, child);
  }

}

void loop_work (int seconds)
{
  clock_t until = clock() + (seconds *  CLOCKS_PER_SEC);
  while (clock() < until);

}
