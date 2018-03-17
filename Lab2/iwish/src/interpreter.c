#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include "DynamicByteArray.h"
#include "SafelyFork.h"
#include "parsertokens.h"

#define STDIN_FD 0
#define STDOUT_FD 1

//returns 0 on success, -1 on failure.
int handleRedirection(Redirection *redirection)
{
  if(redirection == NULL)
    return -1;

  int file;
  if(redirection->isInputRedirection)
    file = open(redirection->word, O_RDONLY);
  else
    file = creat(redirection->word, 0664); //permissions: -rw-rw-r--

  if(file < 0)
  {
    perror("creat() failed with error");
    return -1;
  }
  if(dup2(file, redirection->isInputRedirection ? STDIN_FD : STDOUT_FD) < 0)
  {
    perror("dup2() failed with error");
    return -1;
  }

  return 0;
}

//returns 0 on success, -1 on failure.
int handleCommandlineArgument(char *arg, DynamicByteArray *dynArr)
{
  return DBA_addElemToArr(dynArr, &arg);
}

//returns 0 if successful, returns -1 otherwise.
int handleCommandElementList(Simple_Command_Element_List *list, DynamicByteArray *dynArr)
{
  if(list == NULL)
    return 0;

  //TODO: do stuff
  if((list->element).isRedirection)
  {
    if(handleRedirection((list->element).redirection_pointer) < 0)
      return -1;
  }
  else
  {
    if(handleCommandlineArgument((list->element).word, dynArr) < 0)
      return -1;
  }

  return handleCommandElementList(list->next, dynArr);
}


//TODO: check if we need to keep track of all child-processes we're making,
//      in case the shell receives a SIG_INT and it needs to terminate all
//      of its living children.
//returns 0 if successful, returns -1 otherwise.
//if incomingPipe < 0, that pipe will not be connected.
int handleCommandList(Simple_Command_List *list, int incomingInputPipe, int incomingOutputPipe)
{
  if(list == NULL)
    return -1;

  Simple_Command_Element_List *element_list = (list->simple_command).element_list;

  //connect incoming input and output pipes
  if(incomingInputPipe >= 0)
  {
    if(dup2(incomingInputPipe, STDIN_FD) < 0)
    {
      perror("failed to connect incoming pipe");
      return -1;
    }
  }
  if(incomingOutputPipe >= 0)
  {
    if(dup2(incomingOutputPipe, STDOUT_FD) < 0)
    {
      perror("failed to connect outgoing pipe");
      return -1;
    }
  }

  //search for commandline args and handle i/o redirections.
  //recursively handle the current_element_list:
  //  -handle redirections
  //  -add commandline arguments to some kind of array.
  DynamicByteArray *commandlineArguments = DBA_makeNewDynamicByteArray(sizeof(char*));
  if(handleCommandElementList(element_list, commandlineArguments) < 0)
  {
    free(commandlineArguments);
    return -1;
  }

  //execute
  char *nullPtr = NULL; //needed, as just passing NULL will result in an invalid read.
  DBA_addElemToArr(commandlineArguments, &nullPtr); //finish newargv with a terminating NULL pointer.
  char **newargv = (char **) DBA_getInternalArray(commandlineArguments);
  free(commandlineArguments);
  execvp(newargv[0], newargv);

  //if we get here, execve failed
  perror("execve() failed with error");
  //TODO: clean up everything
  free(newargv);
  return -1;
}

//cleans up a pipe. does nothing if any file descriptor in that pipe is set to -1.
void closePipe(int *pipe)
{
  if(pipe[0] != -1 && pipe[1] != -1)
  {
    close(pipe[0]);
    close(pipe[1]);
  }
}

//returns EXIT_success if successful, returns EXIT_FAILURE otherwise.
int interpretSimpleList(Simple_List *list)
{
  if(list == NULL)
  {
    fprintf(stderr, "error in interpretSimpleList: cannot interpret NULL\n");
    return EXIT_FAILURE;
  }

  Simple_Command_List *current_command_list = (list->pipeline).simple_command_list;
  int incomingPipe[2] = {-1, -1}; //beginning of pipeline, so no incoming pipe.
  int outgoingPipe[2] = {-1, -1};

  //initiate all commands in the pipeline.
  while(current_command_list != NULL)
  {
    //check whether we need to create a pipe for the next part of the pipeline.
    if(current_command_list->next != NULL)
    {
      if(pipe(outgoingPipe) < 0)
      {
        closePipe(incomingPipe);
        closePipe(outgoingPipe);
        return EXIT_FAILURE;
      }
    }
    else
    outgoingPipe[0] = (outgoingPipe[1] = -1); //end of pipeline, so no outgoing pipe.

    int child_pid = safelyFork();
    if(child_pid < 0)
    {
      closePipe(incomingPipe);
      closePipe(outgoingPipe);
      return EXIT_FAILURE;
    }
    else if(child_pid == 0)
    {
      //Child
      //close unused pipe ends
      if(incomingPipe[1] != -1) close(incomingPipe[1]);
      if(outgoingPipe[0] != -1) close(outgoingPipe[0]);
      handleCommandList(current_command_list, incomingPipe[0], outgoingPipe[1]);

      //if we get here, execution of the command failed.
      return EXIT_FAILURE;
    }
    else
    {
      //Parent
      //close unused pipe ends
      if(incomingPipe[0] != -1) close(incomingPipe[0]);
      if(outgoingPipe[1] != -1) close(outgoingPipe[1]);

      //switch around input and output pipes for the next command
      int temp[2];
      temp[0] = incomingPipe[0];
      temp[1] = incomingPipe[1];
      incomingPipe[0] = outgoingPipe[0];
      incomingPipe[1] = outgoingPipe[1];
      outgoingPipe[0] = temp[0];
      outgoingPipe[1] = temp[1];
      //NOTE: this rather roundabout way is necessary, as (static) array types are not assignable.

      current_command_list = current_command_list->next;
    }
  }

  int exitstatus = EXIT_SUCCESS;
  int wstatus;

  //wait for all children to finish.
  while(1)
  {
    if(wait(&wstatus) == -1)
      break; //no more children to wait for.
    if(WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == EXIT_FAILURE)
      exitstatus = EXIT_FAILURE; //exit and by doing so, send a SIGINT to all children.
  }

  return exitstatus;
}
