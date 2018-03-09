#ifndef DYNAMICSTRING_H
#define DYNAMICSTRING_H

typedef struct DynamicString {
  char *string;
  size_t length;
  size_t capacity;
} Dynamic_String;

//returns 0 on success, -1 on failure
int makeDynamicString(DynamicString *dest);

//returns 0 on success, -1 on failure.
int appendCharToDynString(DynamicString *dynString, const char c);

#endif
