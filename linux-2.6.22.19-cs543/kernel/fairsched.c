#include <linux/fairsched.h>
#include <linux/kernel.h>


unsigned int fair_share_slice(struct task_struct *p)
{
  struct task_struct *ptr = NULL;
  unsigned int fair_share_stat_arr[FAIR_SHARE_USERS] = {0};
  unsigned int num_users = 0;

  int i;

  for (i=0; i < FAIR_SHARE_USERS; i++)
    fair_share_stat_arr[i] = 0;

  for_each_process(ptr)
  {
    if (fair_share_stat_arr[ptr->euid] == 0) num_users++;
    fair_share_stat_arr[ptr->euid]++;
  }
  if (!num_users || !fair_share_stat_arr[p->euid])
    return SLICE_CONSTANT;

  unsigned int processes = fair_share_stat_arr[p->euid];

  unsigned int ts = (SLICE_CONSTANT / num_users) / processes; 
  return ts;

}

