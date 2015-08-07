#include <linux/kmailbox.h>

static LIST_HEAD(kmailbox_queue);
static spinlock_t sp_lock = SPIN_LOCK_UNLOCKED;


int enqueue_kmailbox_msg(struct kmailbox_msg *msg)
{

  unsigned long flags;
  spin_lock_irqsave(&sp_lock, flags);
  printk("Enqueueing:\n");
  printk("from_pid: %d\n", msg->from_pid);
  printk("to_pid: %d\n", msg->to_pid);
  printk("message: %s\n", msg->msg_buf);

  if (!msg)
  {
    spin_unlock_irqrestore(&sp_lock,flags);
    return -1;
  }
  list_add_tail(&msg->msg_queue, &kmailbox_queue);

  spin_unlock_irqrestore(&sp_lock,flags);
  return 0;
}

int dequeue_kmailbox_msg(struct kmailbox_msg **msg, pid_t to)
{

  unsigned long flags;
  spin_lock_irqsave(&sp_lock, flags);

  struct list_head *found = NULL, *tmp = NULL;
  struct kmailbox_msg *found_entry = NULL;

  list_for_each_safe(found, tmp, &kmailbox_queue)
  {
    struct kmailbox_msg *this_entry = list_entry(found, struct kmailbox_msg, msg_queue);
    if (this_entry->to_pid == to && NULL == found_entry)
    {
      found_entry = this_entry;
      list_del(found);
    }
  }

  if (NULL == found_entry)
  {
    spin_unlock_irqrestore(&sp_lock,flags);
    return -1;
  }
  *msg = found_entry;

  printk("Dequeueing:\n");
  printk("from_pid: %d\n", (*msg)->from_pid);
  printk("to_pid: %d\n", (*msg)->to_pid);
  printk("message: %s\n", (*msg)->msg_buf);


  spin_unlock_irqrestore(&sp_lock,flags);
  return 0;
}


int dequeue_kmailbox_msg_pid(struct kmailbox_msg **msg, pid_t to, pid_t from)
{

  unsigned long flags;
  spin_lock_irqsave(&sp_lock, flags);

  struct list_head *found = NULL, *tmp = NULL;
  struct kmailbox_msg *found_entry = NULL;

  list_for_each_safe(found, tmp, &kmailbox_queue)
  {
    struct kmailbox_msg *this_entry = list_entry(found, struct kmailbox_msg, msg_queue);
    if (this_entry->to_pid == to && this_entry->from_pid == from && NULL == found_entry)
    {
      found_entry = this_entry;
      list_del(found);
    }
  }

  if (NULL == found_entry)
  {
    spin_unlock_irqrestore(&sp_lock,flags);
    return -1;
  }
  *msg = found_entry;

  printk("Dequeueing:\n");
  printk("from_pid: %d\n", (*msg)->from_pid);
  printk("to_pid: %d\n", (*msg)->to_pid);
  printk("message: %s\n", (*msg)->msg_buf);

  spin_unlock_irqrestore(&sp_lock,flags);
  return 0;
}
