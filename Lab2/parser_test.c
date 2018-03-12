#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "parser.h"
#include "DynamicString.h"
#include "DynamicArray.h"

//reads input from stream and returns a c-string.
//returns NULL on failure.
//completely skips over "\\\n", to save our parser some work.
//callers must free the allocated string.
char * readInput(FILE *stream)
{
  DynamicString *string_ptr = makeDynamicString();
  if(string_ptr == NULL)
  {
    fprintf(stderr, "error in readInput: could not make a dynamic string\n");
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
        appendCharToDynString(string_ptr, c);
        appendCharToDynString(string_ptr, c2);
      }
      else
      {
        continue;
      }
    }
    appendCharToDynString(string_ptr, c);
  }
  appendCharToDynString(string_ptr, '\0');

  char *retval = string_ptr->string;
  //cleanup as we no longer use string_ptr anymore.
  free(string_ptr);
  return retval;
}

/* string destructor for use with DYNARR_makeNewArray.
 * since we are supposed to use the c99 compiler and not the gcc compiler,
 * this function is defined globally instead of inside functions calling
 * DYNARR_makeNewArray.
 */
void myStringDestructor(void * string)
{
  free(string);
}

//will split input into multiple strings.
//ends array with a NULL pointer.
//returns NULL on failure.
char **tokenise(char *input)
{
  DynamicArray *arr = DYNARR_makeNewArray(sizeof(char *), myStringDestructor);
  if(arr == NULL)
    return NULL;

  char *token = strtok(input, " ");
  char *buffer;
  int persistance; //exists for the sole purpose of code readability

  while(1) //breaks when token == NULL, after storing a terminating NULL pointer in our array.
  {
    if(token != NULL)
    {
      buffer = malloc((strlen(token) +1) * sizeof(char));
      if(buffer == NULL)
      {
        perror("malloc");
        DYNARR_destroyArray(arr, persistance=0);
        return NULL;
      }
      strcpy(buffer, token);
    }

    if(DYNARR_addElem(arr, token == NULL ? NULL : (void *) buffer) < 0)
    {
      fprintf(stderr, "error in tokenise: failed DYNARR_addElem\n");
      DYNARR_destroyArray(arr, persistance=0);
      free(buffer);
      return NULL;
    }

    if(token == NULL)
      break;
    token = strtok(NULL, " ");
  }
  return (char **) DYNARR_destroyArray(arr, persistance=1);
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
  char *input, *input_copy, **strings;
  input = readInput(stdin);
  if(input == NULL)
  {
    exit(EXIT_FAILURE);
  }

  // printf("input = %s\n", input);

  //for debugging
  input_copy = malloc((strlen(input)+1) * sizeof(char));
  if(input_copy == NULL)
  {
    perror("malloc");
    free(input);
    exit(EXIT_FAILURE);
  }
  strcpy(input_copy, input);

  strings = tokenise(input_copy);
  if(strings == NULL)
  {
    exit(EXIT_FAILURE);
  }


  Simple_List *simple_list = makeNewSimpleList();
  //for debugging
  fprintf(stdout, "\"%s\" is valid: %s\n", input, parseInput(strings, simple_list) ? "true" : "false");

  int i = 0;
  while(strings[i] != NULL)
  {
    printf("strings[%d] = %s\n", i, strings[i]);
    ++i;
  }

  printSimpleList(simple_list);

  freeSimpleList(simple_list);
  free(input);
  free(input_copy);

  i = 0;
  while(strings[i] != NULL)
  {
    free(strings[i]);
    ++i;
  }
  free(strings);
  return 0;
}
