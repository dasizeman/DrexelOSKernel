#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/linkage.h>
#include <linux/spinlock.h>
#include <linux/list.h>

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

asmlinkage unsigned int sys_swipe(pid_t target, pid_t victim)
{
  struct task_struct *target_p = NULL, *victim_p = NULL;

  if (target == victim)
    return 0;
  
  // Find the target
  for_each_process(target_p)
    if (target_p->tgid == target)
      break;

  // Find the victim
  for_each_process(victim_p)
    if (victim_p->tgid == victim)
      break;

  if (target_p == NULL || victim_p == NULL)
    return 0;

  unsigned int stolen = 0;

  // Lock, since we're messing with task structs
  spinlock_t sp_lock = SPIN_LOCK_UNLOCKED;
  unsigned long flags;
  spin_lock_irqsave(&sp_lock, flags);

  // First steal the victim's timeslice
  stolen += victim_p->time_slice;
  victim_p->time_slice = 0;

  // Then iterate through its children and steal theirs
  struct list_head *position = NULL;
  struct task_struct *data_p = NULL;
  list_for_each(position, &victim_p->children)
  {
    data_p = list_entry(position, struct task_struct, sibling);
    stolen += data_p->time_slice;
    data_p->time_slice = 0;
  }

  // Give the stolen time to the target
  target_p->time_slice += stolen;

  // Unlock
  spin_unlock_irqrestore(&sp_lock, flags);

  return stolen;
}
