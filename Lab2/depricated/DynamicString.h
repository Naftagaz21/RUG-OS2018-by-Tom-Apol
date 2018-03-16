#ifndef DYNAMICSTRING_H
#define DYNAMICSTRING_H

// #include <stdlib.h>
typedef struct DynamicString {
  char *string;
  size_t length;
  size_t capacity;
} DynamicString;

//returns a DynamicString pointer on success, NULL on failure
DynamicString * makeDynamicString();

//returns 0 on success, -1 on failure.
int appendCharToDynString(DynamicString *dynString, const char c);

#endif
