#include <linux/fairsched.h>
static unsigned int num_users;
static unsigned int fair_share_stat_arr[FAIR_SHARE_USERS];


static void update_data()
{
  struct task_struct *ptr = NULL;
  num_users = 0;

  int i;

  for (i=0; i < FAIR_SHARE_USERS; i++)
    fair_share_stat_arr[i] = 0;

  for_each_process(ptr)
  {
    if (fair_share_stat_arr[ptr->uid] == 0) num_users++;
    fair_share_stat_arr[ptr->uid]++;
  }


}

unsigned int fair_share_slice(struct task_struct *p)
{
  update_data();
  return (unsigned int)((SLICE_CONSTANT / num_users) / fair_share_stat_arr[p->uid]);
}

int generate_stat_buffer(char *buffer)
{
  update_data();
  // A line for each user
  char *final;
  char *lines = (char*)kmalloc((num_users) * sizeof(char*), GFP_KERNEL);
  unsigned int user_time = SLICE_CONSTANT / num_users;
  sprintf(final, "Time for each user: %d\n", user_time);
  int i;
  int idx = 0;
  for (i = 0; i < FAIR_SHARE_USERS; i++)
  {
    if (fair_share_stat_arr[i] == 0)
    {
      idx++;
      continue;
    }
    unsigned int numprocs = fair_share_stat_arr[i];
    unsigned int proctime = user_time / numprocs;
    sprintf(lines[idx], "User:%d\nRunning %d processes\nProcess time slice: %d\n", i, numprocs, proctime);
    strcat(final, lines[idx]);
  }

  buffer = final;
  return strlen(final);
}
