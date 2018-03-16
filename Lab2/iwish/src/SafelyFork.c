#include "SafelyFork.h"

int safelyFork()
{
  int original_parent_pid = getpid();
  int child_pid = fork();
  if(child_pid < 0)
  {
    perror("error in safelyFork");
    return -1;
  }
  if(child_pid == 0)
  {
    //Child
    //Make sure that when the parent terminates, all children terminate as well.
    if(prctl(PR_SET_PDEATHSIG, SIGTERM) < 0)
    {
      perror("error in safelyFork");
      return -1;
    }
    //If the parent already terminated before the previous code executed,
    // its still running children have been adopted by init. (or any closer subreaper process)
    if(getppid() != original_parent_pid)
    {
      fprintf(stderr, "error in safelyFork: parent has already terminated. terminating child.\n");
      return -1;
    }
  }
  return child_pid;
}
