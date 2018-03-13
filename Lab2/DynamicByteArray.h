#ifndef DYNAMICBYTEARRAY_H
#define DYNAMICBYTEARRAY_H

#include <stdlib.h>

#define DBA_INITSIZE 16

typedef struct DynamicByteArray {
  size_t type_size;
  size_t nr_of_elements;
  size_t capacity;
  char * elements;
} DynamicByteArray;

//returns NULL on failure.
DynamicByteArray * DBA_makeNewDynamicByteArray(size_t type_size);

//returns 0 on success, -1 on failure.
int DBA_resizeArr(DynamicByteArray *arr, size_t new_nr_of_elements);

//returns 0 on success, -1 on failure.
int DBA_addElemToArr(DynamicByteArray *arr, void *bytestring);

/* Base function that returns a void pointer to the place in the byte array of
 * this particular index. Should be manually cast to an appropriate type pointer
 * and dereferenced for use.
 */
void * DBA_getElem(DynamicByteArray *arr, size_t index)

/* Standard functions for retrieving elements with a standard type. */
char DBA_getElem_CHAR(DynamicByteArray *arr, size_t index);
int DBA_getElem_INT(DynamicByteArray *arr, size_t index);
long DBA_getElem_LONG(DynamicByteArray *arr, size_t index);
float DBA_getElem_FLOAT(DynamicByteArray *arr, size_t index);
double DBA_getElem_DOUBLE(DynamicByteArray *arr, size_t index);
void * DBA_getElem_VOIDPTR(DynamicByteArray *arr, size_t index);

#endif
