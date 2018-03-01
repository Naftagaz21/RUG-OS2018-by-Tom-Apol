#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/wait.h>

/* child processes have their own data segment, simulate shared memory */

/* for now:
   - when a child needs to update their data segment (= variable), send a signal to the parent.
   the parent then sends the actual value via a pipe.

   - when a child has altered their data segment (= variable), write it to the pipe,
   and send a signal to the parent, which then signals the other child to become active.

   This might not be allowed in the future, but as far as we know, this is allowed for now.
 */


/* mask signals of SIGUSR1 and SIGUSR2 for I_Want_To_Update and I_Have_Updated signals. */

/* sa_mask: somehow use this to not get interrupted while you're in the signal handler */

int main(int argv, char* argv[])
{


  int parent, child1, child2, status;
  child1 = fork();
  if(child1 < 0)
  {
    perror("child1: fork");
    exit(EXIT_FAILURE);
  }
  else if
  (child1 != 0)
  {
    child2 = fork();
    if(child2 < 0)
    {
      perror("child2: fork");
      exit(EXIT_FAILURE);
    }
    if(child2 != 0)
    {
      /* parent process here */
      
    }
    else
    {
      /* child2 process here */

    }
  }
  else
  {
    /* child1 process here */

  }

  return EXIT_SUCCESS;
}
