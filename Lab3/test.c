#include <lib.h>
#include <stdio.h>

int main(void)
{
  message m;

  _syscall(PM_PROC_NR, UTCTIME, &m);
  printf("Message_type = %d.\n", m.m_type);
  printf("Current utctime is %ld.\n", m.m_u.m_m10.m10l1);
  return 0;
}
