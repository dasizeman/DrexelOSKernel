#include "kmailbox_usr.h"


void mysend(pid_t pid, unsigned int n, void* buf)
{
  syscall(292, pid, n, buf);
}

unsigned int myreceive(pid_t pid, unsigned int n, void* buf)
{
  return syscall(293, pid, n, buf);
}
