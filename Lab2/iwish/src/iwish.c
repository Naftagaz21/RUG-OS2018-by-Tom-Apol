
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include "parser.h"
#include "DynamicByteArray.h"
#include "interpreter.h"
#include "SafelyFork.h"

/* starts the interpreter which also executes the command.
 * forks for the following reasons:
 * - if we background a process
 */
void interpretInput(char **strings, char* input)
{
  int child_pid = safelyFork();
  if(child_pid < 0)
  {
    free(strings);
    free(input);
    printf("error: could not fork. exiting shell\n");
    exit(EXIT_FAILURE);
  }
  if(child_pid == 0)
  {
    //Child
    Simple_List *simple_list = makeNewSimpleList();
    int isValidInput = parseInput(strings, simple_list);
    int exitStatus = 0;

    if(isValidInput)
      exitStatus = interpretSimpleList(simple_list);
    else
      printf("error: invalid input\n");
    freeSimpleList(simple_list);
    free(strings);
    free(input);
    exit(exitStatus);
  }
  //Parent (Shell itself)
  wait(NULL);
}

/* INPUT HANDLING */

//reads input from stream and returns a c-string.
//returns NULL on failure.
//completely skips over "\\\n", to save our parser some work.
//callers must free the allocated string.
char * readInput(FILE *stream)
{

  DynamicByteArray *string_ptr = DBA_makeNewDynamicByteArray(sizeof(char));
  if(string_ptr == NULL)
  {
    fprintf(stderr, "error in readInput: could not make a dynamic bytearray\n");
    return NULL;
  }

  char c, c2;
  while((c = fgetc(stream)) != '\n') //a lone \n signals the end of the input
  {
    //disregard "\\\n" sequences,
    if(c == '\\')
    {
      if((c2 = fgetc(stream)) != '\n')
      {
        DBA_addElemToArr(string_ptr, &c);
        DBA_addElemToArr(string_ptr, &c2);
      }
      else
      {
        continue;
      }
    }
    DBA_addElemToArr(string_ptr, &c);
  }
  DBA_addElemToArr(string_ptr, "");

  char *retval = (char*) string_ptr->elements;
  //cleanup as we no longer use string_ptr anymore.
  free(string_ptr);
  return retval;
}


//will split input into multiple strings.
//ends array with a NULL pointer.
//returns NULL on failure.
char **tokenise(char *input)
{
  DynamicByteArray *arr = DBA_makeNewDynamicByteArray(sizeof(char*));
  if(arr == NULL)
    return NULL;

  char *token = strtok(input, " ");

  while(1) //breaks when token == NULL, after storing a terminating NULL pointer in our array.
  {
    if(DBA_addElemToArr(arr, &token) < 0)
    {
      fprintf(stderr, "error in tokenise: failed DYNARR_addElem\n");
      free(arr);
      return NULL;
    }

    if(token == NULL)
      break;
    token = strtok(NULL, " ");
  }
  char **retval = (char**) arr->elements;
  free(arr);
  return retval;
}

/* OTHER */

//exit under these commands
int stringIsExitKeyword(char *string)
{
  if(string != NULL)
  {
    if(strcmp(string, "exit") == 0 || strcmp(string, "quit") == 0 || strcmp(string, "q") == 0)
      return 1;
  }
  return 0;
}

void printWelcome()
{
  printf("Welcome to the iwish-shell. This minimal shell follows a bash-like syntax and only supports the following operations:\n");
  printf("  - Executing programs found in $PATH or outside of it given the correct filepath.\n");
  printf("  - Standard input and output redirection with the \"< input.file\" and \"> output.file\" syntax.\n");
  printf("  - Background processes with the \"command &\" syntax.\n");
  printf("  - Piping with the \"a.out | b.out | c.out\" syntax.\n");
  printf("Note that this shell expects at least one whitespace between each token.\n (e.g. \"a.out <in.file\" is considered invalid input)\n");
  printf("Furthermore, a newline can be started anywere, by using \"\\<\\n>\", where \"<\\n>\" represents a newline.\n (e.g. \"a.o\\<\\n>ut\" is considered valid input)\n");
  printf("Type \"exit\", \"quit\" or \"q\" to exit the shell.\n\n");
}

/* MAIN */

int main(int argc, char *argv[])
{
  printWelcome();

  char *input = NULL, **strings = NULL;
  while(1)
  {
    printf("iwish $ ");
    input = readInput(stdin);
    if(input == NULL)
    {
      exit(EXIT_FAILURE);
    }

    strings = tokenise(input);
    if(strings == NULL)
    {
      free(input);
      exit(EXIT_FAILURE);
    }

    if(stringIsExitKeyword(strings[0]))
      if(strings[1] == NULL)
      {
        printf("iwish $ exiting iwish-shell\n");
        break;
      }

    interpretInput(strings, input);

    free(strings);
    free(input);
  }

  free(strings);
  free(input);

  return 0;
}
