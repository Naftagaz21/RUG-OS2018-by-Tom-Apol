#include <lib.h> //syscall and message
#include <time.h> //function prototypes
#include <stdio.h> //for debugging purposes

time_t gettime()
{
  message m;
  if(_syscall(PM_PROC_NR, TIME, &m) < 0)
    return -1;
  time_t utctime = m.m2_l1;
  return utctime;
}

int main(void)
{
  printf("Current do_time is %d.\n", gettime());
  printf("Current librarycall to utctime is %d.\n", utctime());
  return 0;
}
