#include <lib.h>
#include <time.h>

//if tloc is non-NULL, store utctime in tloc as well.
time_t utctime(time_t *tloc)
{
  message m;

  if(_syscall(PM_PROC_NR, UTCTIME, &m) < 0)
    return -1;

  if(tloc != NULL)
    *tloc = m.m2_l1;
  return m.m2_l1;
}
