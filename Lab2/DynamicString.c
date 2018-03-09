#include <stdlib.h>
#include <stdio.h>
#include "DynamicString.h"

#define DYNAMIC_STRING_START_LENGTH 16

//returns a DynamicString pointer on success, NULL on failure
DynamicString * makeDynamicString()
{
  DynamicString *dynString = malloc(sizeof(DynamicString));
  if(dynString == NULL)
  {
    perror("malloc");
    return NULL;
  }

  dynString->string = malloc(DYNAMIC_STRING_START_LENGTH * sizeof(char));
  if(dynString->string == NULL)
  {
    free(dynString);
    perror("malloc");
    return NULL;
  }

  dynString->length = 0;
  dynString->capacity = DYNAMIC_STRING_START_LENGTH;

  return dynString;
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
  ++dynString->length;
  return 0;
}
