#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/linkage.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/wait.h>
#include <linux/custom_syscalls.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <asm/uaccess.h>
#include <linux/kmailbox.h>

static inline loff_t file_pos_read(struct file *file)
{
	return file->f_pos;
}

static inline void file_pos_write(struct file *file, loff_t pos)
{
	file->f_pos = pos;
}

/* wait queue for myjoin */
static wait_queue_head_t myjoin_queue;

void myjoin_wake_up ()
{
  wake_up(&myjoin_queue);

}

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
    if ((int)(data_p->time_slice) > 0)
    {
      stolen += data_p->time_slice;
      printk("Adding %d ns from %d\n", data_p->time_slice, data_p->tgid);
    }
    data_p->time_slice = 0;
  }

  // Give the stolen time to the target
  target_p->time_slice += stolen;

  // Unlock
  spin_unlock_irqrestore(&sp_lock, flags);

  return stolen;
}

asmlinkage int sys_zombify(pid_t victim)
{
  struct task_struct *p = NULL;

  for_each_process(p)
  {
    if (p->tgid == victim)
    {
      spinlock_t sp_lock = SPIN_LOCK_UNLOCKED;
      unsigned long flags;
      spin_lock_irqsave(&sp_lock, flags);

      p->state = TASK_UNINTERRUPTIBLE;
      p->exit_state = EXIT_ZOMBIE;

      spin_unlock_irqrestore(&sp_lock, flags);

      return 0;
    }

  }

  return -1;

}

asmlinkage int sys_myjoin(pid_t target)
{
  // Lock so target doesn't exit
  spinlock_t sp_lock = SPIN_LOCK_UNLOCKED;
  unsigned long flags;
  spin_lock_irqsave(&sp_lock, flags);


  // Sanity checks - target exists and has not exited
  struct task_struct *target_p = NULL;
  for_each_process(target_p)
  {
    if (target_p->tgid == target)
      break;
  }

  if (target_p == NULL)
    return -1;

  if (target_p->exit_state == EXIT_ZOMBIE || target_p->exit_state == EXIT_DEAD)
    return -1;


  // Initialize our wait queue
  init_waitqueue_head(&myjoin_queue);

  // Let target know to wake us up when it exits
  target_p->myjoin_pid = current->tgid;

  // Go to sleep
  sleep_on(&myjoin_queue);

  // Unlock
  spin_unlock_irqrestore(&sp_lock, flags);

  return 0;
}


asmlinkage ssize_t sys_forcewrite(unsigned int fd, const char __user * buf, size_t count)
{
	struct file *file;
	ssize_t ret = -EBADF;
	int fput_needed;

	file = fget_light(fd, &fput_needed);
	if (file) {
		loff_t pos = file_pos_read(file);
		ret = vfs_write_force(file, buf, count, &pos);
		file_pos_write(file, pos);
		fput_light(file, fput_needed);
	}

	return ret;
}

asmlinkage void sys_mysend(pid_t pid, unsigned int n, void* buf)
{
  void *kspace_buffer = NULL;
  copy_from_user(kspace_buffer, buf, n);

  // Create the message struct
  struct kmailbox_msg *msg = (struct kmailbox_msg*)kmalloc(sizeof(struct kmailbox_msg), GFP_KERNEL);
  if (NULL == msg)
  {
    printk("Failed to malloc kmailbox_msg struct!\n");
    return;
  }

  msg->from_pid = current->tgid;
  msg->to_pid = pid;
  msg->msg_buf = kspace_buffer;
  INIT_LIST_HEAD(&msg->msg_queue);

  // Enqueue the message
  if (enqueue_kmailbox_msg(msg) < 0)
  {
    printk("enqueue_kmailbox_msg failed!\n");
  }
}

asmlinkage unsigned int sys_myreceive(pid_t pid, unsigned int n, void* buf)
{
  struct kmailbox_msg* recvd_message = NULL;

  if ((int)pid > 0)
  {
    if (dequeue_kmailbox_msg_pid(&recvd_message, pid) < 0)
      return 0;
  }
  else
  {
    if (dequeue_kmailbox_msg(&recvd_message) < 0)
      return 0;
  }

  copy_to_user(buf, recvd_message->msg_buf, n);
  return n;

}
