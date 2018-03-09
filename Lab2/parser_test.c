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
  DynamicString *string_ptr;
  if(makeDynamicString(string_ptr) < 0)
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
        appendCharToDynString(c);
        appendCharToDynString(c2);
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
void myStringDestructor(char *string)
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

    if(DYNARR_addElem(arr, (void *) buffer) < 0)
    {
      fprintf("error in tokenise: failed DYNARR_addElem");
      DYNARR_destroyArray(arr, persistance=0);
      free(buffer);
      return NULL;
    }

    if(token == NULL)
      break;
  }
  return (char *) DYNARR_destroyArray(arr, persistance=1);
}


int main(int argc, char *argv[])
{
  char *input, *input_copy, **strings;
  input = readInput(stdin);
  if(input == NULL)
  {
    exit(EXIT_FAILURE);
  }

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

  //for debugging
  fprintf(stdout, "\"%s\" is valid: %s\n", input, parseInput(strings) ? "true" : "false");

  free(input);
  free(input_copy);

  free(strings);
  return 0;
}
