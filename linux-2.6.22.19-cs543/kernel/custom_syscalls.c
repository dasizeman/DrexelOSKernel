#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/linkage.h>
#include <linux/spinlock.h>

asmlinkage long sys_mygetpid(void)
{
  return current->tgid;
}

asmlinkage int sys_steal(pid_t pid)
{
  struct task_struct *p = NULL;

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

asmlinkage unsigned int sys_quad(pid_t pid)
{
  struct task_struct *p = NULL;

  for_each_process(p)
  {
    if (p->tgid == pid)
    {
      spinlock_t sp_lock = SPIN_LOCK_UNLOCKED;
      unsigned long flags;
      spin_lock_irqsave(&sp_lock, flags);
      p->time_slice *= 4;
      spin_unlock_irqrestore(&sp_lock, flags);
      return p->time_slice;
    }

  }

  return 0;

}
