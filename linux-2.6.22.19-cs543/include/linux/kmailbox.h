#include <linux/list.h>
#include <linux/sched.h>
#include <linux/spinlock.h>

struct kmailbox_msg
{
  struct list_head msg_queue;
  long from_pid;
  long to_pid;

  void *msg_buf;

};

/**
 * Add a new message to the delivery queue
 * msg : The message to queue
 * Return:
 * 0 on success, -1 on failure
 */
int enqueue_kmailbox_msg(struct kmailbox_msg *msg);

/**
 * Get the next available message on the queue
 * msg : A pointer to the retrieved message
 * Return:
 * 0 on success, -1 on failure
 */
int dequeue_kmailbox_msg(struct kmailbox_msg **msg);

/**
 * Get the next available message to the specified
 * pid on the queue
 * msg : A pointer to the retrieved message
 * to  : The pid of the recipient of the message
 * Return:
 * 0 on success, -1 on failure
 */
int dequeue_kmailbox_msg_pid(struct kmailbox_msg **msg, pid_t to);
