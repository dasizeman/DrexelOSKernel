#include <linux/list.h>

struct kmailbox_msg
{
  struct list_head msg_queue;
  long from_pid;
  long to_pid;

  void *msg_buf

};

int enqueue_kmailbox_msg(struct kmailbox_msg *msg);

int dequeue_kmailbox_msg(struct kmailbox_msg *msg);
