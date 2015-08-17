#include <linux/sched.h>
#include <linux/list.h>

#define FAIR_SHARE_USERS 101
#define USER_OFFSET 999  // Non-root uids have euids starting at 1000
#define SLICE_CONSTANT 500

unsigned int fair_share_slice(struct task_struct *p);

