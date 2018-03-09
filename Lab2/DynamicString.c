#include <stdlib.h>
#include <stdio.h>
#include "DynamicString.h"

#define DYNAMIC_STRING_START_LENGTH 16

typedef struct DynamicString {
  char *string;
  size_t length;
  size_t capacity;
} Dynamic_String;

//returns 0 on success, -1 on failure
int makeDynamicString(DynamicString *dest)
{
  dest = malloc(sizeof(DynamicString));
  if(dest == NULL)
  {
    perror("malloc");
    return -1;
  }

  dest->string = malloc(DYNAMIC_STRING_START_LENGTH * sizeof(char));
  if(dest->string == NULL)
  {
    free(dynString);
    perror("malloc");
    return -1;
  }

  dest->length = 0;
  dest->capacity = DYNAMIC_STRING_START_LENGTH;

  return 0;
}

//returns 0 on success, -1 on failure.
int appendCharToDynString(DynamicString *dynString, const char c)
{
  if(dynString == NULL)
    return -1;

  //double dynString's capacity if there is no more space
  if(dynString->length >= dynString->capacity)
  {
    char* retVal = realloc(dynString->string, dynString->capacity*2);
    if(retVal == NULL)
      return -1;

    dynString->string = retVal;
    dynString->capacity *= 2;
  }

  dynString->string[dynString->length] = c;

  return 0;
}
