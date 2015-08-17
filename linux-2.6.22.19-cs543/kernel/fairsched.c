#include <linux/fairsched.h>
#include <linux/kernel.h>


unsigned int fair_share_slice(struct task_struct *p)
{
  /**
   * fair_share_time_slice[i][0] is the number of procs
   * for a user
   * fair_share_time_slice[i][1] is the uid
   */

  struct task_struct *ptr = NULL;
  unsigned int fair_share_stat_arr[FAIR_SHARE_USERS][2] = {{0,0}};
  unsigned int num_users = 0;

  int i;

  unsigned int useridx, thisprocidx;
  unsigned int arr_end = 0;

  for_each_process(ptr)
  {
    // Search for this user in our array (or where they will go)
    for (i = 0; i <= arr_end; i++)
    {
      useridx = i;
      if (fair_share_stat_arr[i][1] == ptr->euid)
        break;
    }

    if (useridx == arr_end)
    {
      arr_end++;
      num_users++;
    }
    fair_share_stat_arr[useridx][0]++;
    fair_share_stat_arr[useridx][1] = ptr->euid;
    if (ptr->euid == p->euid)
      thisprocidx = useridx;

  }
  if (!num_users || !fair_share_stat_arr[thisprocidx])
    return SLICE_CONSTANT;

  unsigned int processes = fair_share_stat_arr[thisprocidx];

  unsigned int ts = (SLICE_CONSTANT / num_users) / processes; 
  return ts;

}

