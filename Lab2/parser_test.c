#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "parser.h"
#include "DynamicByteArray.h"

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

/* ############################################################## */
// TEST FUNCTION FOR CHECKING WHETHER Simple_List WORKS CORRECTLY //
/* ############################################################## */

void printRedirection(Redirection *redirection)
{
  if(redirection == NULL)
    return;
  printf(" %s %s", redirection->isInputRedirection ? "<" : ">", redirection->word);
}

void printSimpleCommandElement(Simple_Command_Element *element)
{
  if(element == NULL)
    return;
  if(element->isRedirection)
  {
    printRedirection(element->redirection_pointer);
  }
  else
  {
    printf(" %s", element->word);
  }
}

void printSimpleCommandElementList(Simple_Command_Element_List *list)
{
  if(list == NULL)
    return;
  printSimpleCommandElement(&(list->element));
  printSimpleCommandElementList(list->next);
}

void printSimpleCommand(Simple_Command *command)
{
  if(command == NULL)
    return;
  printSimpleCommandElementList(command->element_list);
}

void printSimpleCommandList(Simple_Command_List *list)
{
  if(list == NULL)
    return;
  printSimpleCommand(&(list->simple_command));
  printSimpleCommandList(list->next);
}

void printPipeline(Pipeline *pipeline)
{
  if(pipeline == NULL)
    return;
  printSimpleCommandList(pipeline->simple_command_list);
}

void printSimpleList(Simple_List *list)
{
  if(list == NULL)
    return;
  printPipeline(&(list->pipeline));
  if(list->hasDaemonAmpersand) printf(" &");
  putchar('\n');
}

/* ############################################################## */

int main(int argc, char *argv[])
{
  char *input, **strings;
  input = readInput(stdin);
  if(input == NULL)
  {
    exit(EXIT_FAILURE);
  }

  printf("input = %s\n", input);

  strings = tokenise(input);
  if(strings == NULL)
  {
    free(input);
    exit(EXIT_FAILURE);
  }


  Simple_List *simple_list = makeNewSimpleList();
  //for debugging
  fprintf(stdout, "aforementioned input is valid: %s\n", parseInput(strings, simple_list) ? "true" : "false");

  int i = 0;
  while(strings[i] != NULL)
  {
    printf("strings[%d] = %s\n", i, strings[i]);
    ++i;
  }

  printSimpleList(simple_list);

  freeSimpleList(simple_list);
  free(input);
  free(strings);
  return 0;
}
