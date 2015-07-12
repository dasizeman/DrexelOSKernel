#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/linkage.h>

asmlinkage long sys_mygetpid(void)
{
  return current->tgid;
}
