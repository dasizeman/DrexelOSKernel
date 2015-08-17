#include <linux/fairsched.h>
#include <linux/kernel.h>


unsigned int fair_share_slice(struct task_struct *p)
{
  /**
   * Our stat arr of the form [x][y].  y holds the actual uid
   * we will use variables to represent how much of the arr is
   * actually used (could use a linked list I guess), and also 
   * flag the idx of the user for the current task struct */

  struct task_struct *ptr = NULL;
  unsigned int fair_share_stat_arr[FAIR_SHARE_USERS][2] = {{0,0}};
  unsigned int num_users = 0;

  int i;

  //for (i=0; i < FAIR_SHARE_USERS; i++)
    //fair_share_stat_arr[i] = 0;

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

