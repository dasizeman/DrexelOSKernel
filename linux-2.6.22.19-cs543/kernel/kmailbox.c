#include <linux/kmailbox.h>

static LIST_HEAD(kmailbox_queue);


int enqueue_kmailbox_msg(struct kmailbox_msg *msg)
{
  if (!msg)
    return -1;

  list_add_tail(&msg->msg_queue, &kmailbox_queue);
  return 0;
}

int dequeue_kmailbox_msg(struct kmailbox_msg **msg)
{
  if (!kmailbox_queue.next)
    return -1;

  struct list_head *to_remove = NULL;
  to_remove = kmailbox_queue.next;
  *msg = list_entry(to_remove, struct kmailbox_msg, msg_queue);

  list_del(to_remove);
  return 0;
}


int dequeue_kmailbox_msg_pid(struct kmailbox_msg **msg, pid_t to)
{
  struct list_head *found = NULL;
  struct kmailbox_msg *found_entry = NULL;

  list_for_each(found, &kmailbox_queue)
  {
    found_entry = list_entry(found, struct kmailbox_msg, msg_queue);
    if (found_entry->to_pid == to)
      break;
  }

  if (NULL == found_entry)
    return -1;

  *msg = found_entry;

  list_del(found);
  return 0;
}
