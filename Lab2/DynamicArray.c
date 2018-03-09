#include <stdlib.h>
#include "DynamicArray.h"

/* If the provided elemDestructorFunc is NULL,
 * it is assumed that the elements do not have to be freed when destroying the array.
 * Returns NULL if allocation failed.
 */
DynamicArray * DYNARR_makeNewArray(size_t elemSize, void(*elemDestructorFunc)(void *elem))
{
  DynamicArray *arr = malloc(sizeof(DynamicArray));
  if(arr == NULL)
    return NULL;
  arr->elems = malloc(DYNARR_INITSIZE * elemSize);
  if(arr->elems == NULL)
  {
    free(arr);
    return NULL;
  }
  arr->capacity = DYNARR_INITSIZE;
  arr->size = 0;
  arr->elemDestructorFunc = elemDestructorFunc;
  return arr;
}

//If internalArrayPersistence != 0, returns a pointer towards the internal array.
//returns NULL otherwise.
void * DYNARR_destroyArray(DynamicArray *arr, int internalArrayPersistence)
{
  if(internalArrayPersistence)
  {
    void *internalArr = arr->elems;
    free(arr);
    return internalArr;
  }

  if(arr->elemDestructorFunc != NULL)
  for(int i = 0; i < arr->size; ++i){
    arr->elemDestructorFunc(arr->elems[i]);
  }
  free(arr->elems);
  free(arr);
  return NULL;
}

/* When successful, returns 0.
 * On failure, leaves the array intact and returns -1.
 */
int DYNARR_resizeArr(DynamicArray *arr, size_t newCapacity)
{
  void *tempPtr = realloc(arr->elems, newCapacity);
  if(tempPtr == NULL)
    return -1;

  arr->elems = tempPtr;
  arr->capacity = newCapacity;
  return 0;
}


/* When successful, returns 0.
 * On failure, leaves the array intact and returns -1.
 */
int DYNARR_addElem(DynamicArray *arr, void *elem)
{
  if(arr->size == arr->capacity)
  {
    if(DYNARR_resizeArr(arr, 2*arr->size) < 0)
      return -1;
  }
  arr->elems[arr->size] = elem;
  ++arr->size;
  return 0;
}
