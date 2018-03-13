
#include "DynamicByteArray.h"

//returns NULL on failure.
DynamicByteArray * DBA_makeNewDynamicByteArray(size_t type_size)
{
  DynamicByteArray arr = malloc(sizeof(DynamicByteArray));
  if(arr == NULL)
    return NULL;
  arr->elements = malloc(DBA_INITSIZE * type_size);
  if(arr->elements == NULL)
  {
    free(arr);
    return NULL;
  }
  arr->type_size = type_size;
  arr->nr_of_elements = 0;
  arr->capacity = DBA_INITSIZE;
  return arr;
}

//returns 0 on success, -1 on failure.
int DBA_resizeArr(DynamicByteArray *arr, size_t new_nr_of_elements)
{
  char *tempPtr = realloc(arr->elements, new_nr_of_elements * arr->type_size);
  if(tempPtr == NULL)
    return -1;

  arr->elements = tempPtr;
  arr->capacity = newCapacity;
  return 0;
}

//returns 0 on success, -1 on failure.
int DBA_addElemToArr(DynamicByteArray *arr, void *bytestring)
{
  //TODO: resize if needed
  if(arr->nr_of_elements == arr->capacity)
  {
    if(resizeArr(arr, 2*arr->size) < 0)
      return -1;
  }

  for(size_t i = 0; i < type_size; ++i)
  {
    elems[arr->nr_of_elements * arr->type_size + i] = ((char *) bytestring)[i];
  }
  ++arr->nr_of_elements;
  return 0;
}

/* Base function that returns a void pointer to the place in the byte array of
 * this particular index. Should be manually cast to an appropriate type pointer
 * and dereferenced for use.
 */
void * DBA_getElem(DynamicByteArray *arr, size_t index)
{
  return &(arr->elements[index * arr->type_size]);
}


/* Standard functions for retrieving elements with a standard type. */

 char DBA_getElem_CHAR(DynamicByteArray *arr, size_t index)
 {
   return * ((char *) DBA_getElem(arr, index));
 }

int DBA_getElem_INT(DynamicByteArray *arr, size_t index)
{
  return * ((int *) DBA_getElem(arr, index));
}

long DBA_getElem_LONG(DynamicByteArray *arr, size_t index)
{
  return * ((long *) DBA_getElem(arr, index));
}

float DBA_getElem_FLOAT(DynamicByteArray *arr, size_t index)
{
  return * ((float *) DBA_getElem(arr, index));
}

double DBA_getElem_DOUBLE(DynamicByteArray *arr, size_t index)
{
  return * ((double *) DBA_getElem(arr, index));
}

void * DBA_getElem_VOIDPTR(DynamicByteArray *arr, size_t index)
{
  return * ((void **) DBA_getElem(arr, index));
}
