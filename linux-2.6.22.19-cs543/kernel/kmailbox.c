#include <linux/kmailbox.h>

static LIST_HEAD(kmailbox_queue);
static spinlock_t sp_lock = SPIN_LOCK_UNLOCKED;


int enqueue_kmailbox_msg(struct kmailbox_msg *msg)
{

  unsigned long flags;
  spin_lock_irqsave(&sp_lock, flags);

  if (!msg)
  {
    spin_unlock_irqrestore(&sp_lock,flags);
    return -1;
  }
  list_add_tail(&msg->msg_queue, &kmailbox_queue);

  spin_unlock_irqrestore(&sp_lock,flags);
  return 0;
}

int dequeue_kmailbox_msg(struct kmailbox_msg **msg)
{

  unsigned long flags;
  spin_lock_irqsave(&sp_lock, flags);

  if (!kmailbox_queue.next)
  {
    spin_unlock_irqrestore(&sp_lock,flags);
    return -1;
  }
  struct list_head *to_remove = NULL;
  to_remove = kmailbox_queue.next;
  *msg = list_entry(to_remove, struct kmailbox_msg, msg_queue);

  list_del(to_remove);

  spin_unlock_irqrestore(&sp_lock,flags);
  return 0;
}


int dequeue_kmailbox_msg_pid(struct kmailbox_msg **msg, pid_t to)
{

  unsigned long flags;
  spin_lock_irqsave(&sp_lock, flags);

  struct list_head *found = NULL;
  struct kmailbox_msg *found_entry = NULL;

  list_for_each(found, &kmailbox_queue)
  {
    found_entry = list_entry(found, struct kmailbox_msg, msg_queue);
    if (found_entry->to_pid == to)
      break;
  }

  if (NULL == found_entry)
  {
    spin_unlock_irqrestore(&sp_lock,flags);
    return -1;
  }
  *msg = found_entry;

  list_del(found);

  spin_unlock_irqrestore(&sp_lock,flags);
  return 0;
}
