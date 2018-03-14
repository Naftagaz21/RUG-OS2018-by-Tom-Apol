#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "DynamicByteArray.h"

//TODO: make sure the last process in the pipeline has the shell's stdout
//      and as such: don't close the shell's standard streams.

//returns 0 on success, -1 on failure.
int handleRedirection(Redirection *redirection)
{
  //not sure if mode S_IRWXU is the correct mode here
  int file = creat(redirection->word, S_IRWXU);
  if(file < 0)
  {
    //something went wrong
    perror("creat() failed with error");
    return -1;
  }
  if(dup2(file, redirection->isInputRedirection ? stdin : stdout) < 0)
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
//if incomingPipe < 0, it is assumed there is no incoming pipe.
int handleCommandList(Simple_Command_List *list, int incomingInputPipe /*TODO: unfinished */)
{
  if(list == NULL)
    return 0;

  Simple_Command_Element_List *element_list = (list->simple_command).element_list;
  //check that the first element is the file path.
  if((element_list->element).isRedirection)
  {
    fprintf(stderr, "commands must start with a program path!\n");
    return -1;
  }

  //connect incoming input pipe
  if(incomingInputPipe >= 0)
  {
    if(dup2(incomingInputPipe, stdin) < 0)
    {
      perror("dup2() failed with error");
      return -1;
    }
  }

  //search for commandline args and handle i/o redirections.
  //recursively handle the current_element_list:
  //  -handle redirections
  //  -add commandline arguments to some kind of array.
  DynamicByteArray *commandlineArguments = DYNARR_makeNewArray(sizeof(char*));
  if(handleCommandElementList(current_element_list, commandlineArguments) < 0)
  {
    free(commandlineArguments);
    return -1;
  }

  //prepare to pipe
  int outgoingPipe = -1;
  if(list->next != NULL)
  {
    int newPipe[2];
    //init pipe
    if(pipe(newPipe) < 0)
    {
      perror("pipe() failed with error");
      //TODO: do some cleanup
      free(commandlineArguments);
      return -1;
    }

    if(dup2(newPipe[1], stdout) < 0)
    {
      perror("dup2() failed with error");
      //TODO: do some cleanup
      free(commandlineArguments);
      return -1;
    }
    outgoingPipe = newPipe[0];
  }

  //fork and execute
  DBA_addElemToArr(commandlineArguments, &((void*) NULL)); //NOTE: might not compile. tough luck.
  char **newargv = (char **) commandlineArguments->elements;
  free(commandlineArguments);
  int childID = fork();
  if(childID < 0)
  {
    perror("fork() failed with error");
    //TODO: do some cleanup
    return -1;
  }

  else if(childID == 0)
  {
    char const *newenvp[1] = {NULL}; //for posix compatibility, we don't use newenvp = NULL.
    execve(newargv[0], newargv, newenvp);

    //if we get here, execve failed
    perror("execve() failed with error");
    //TODO: clean up everything
    return -1;
  }
  else
  {
    //TODO: add childID to list of (running) children?
    //handle the rest of the pipeline
    return handleCommandList(list->next, outgoingPipe);
  }
}

//returns 0 if successful, returns -1 otherwise.
int interpretSimpleList(Simple_List *list)
{
  if(list == NULL)
  {
    fprintf(stderr, "error in interpretSimpleList: cannot interpret NULL\n");
    return -1;
  }
  Simple_Command_List *current_command_list = (list->pipeline).simple_command_list;
  int retval = handleCommandList(current_command_list, -1)

  //TODO: do stuff with retval
  if(list->hasDaemonAmpersand)
  {
    //TODO: don't wait for children
  }
  else
  {
    //TODO: wait for children
  }
  return 0;
}
