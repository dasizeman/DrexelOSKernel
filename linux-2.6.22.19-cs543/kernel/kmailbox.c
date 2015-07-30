#include <linux/kmailbox.h>

static LIST_HEAD(kmailbox_queue);


int enqueue_kmailbox_msg(struct kmailbox_msg *msg)
{
  if (!msg)
    return -1;

  list_add(&msg->msg_queue, &kmailbox_queue);
}

int dequeue_kmailbox_msg(struct kmailbox_msg *msg)
{

}
