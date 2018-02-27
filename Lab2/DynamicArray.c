#include <stdlib.h>
#include "DynamicArray.h"

/* Each DynamicArray's internal array is initialised with NULLs,
 * such that if filling the array somehow fails,
 * the array can be safely destroyed/freed.
 * Returns NULL if allocation failed.
 */
DynamicArray * DYNARR_makeNewArray(size_t elemSize, void(*elemDestructorFunc)(void *elem))
{
  DynamicArray *arr = malloc(sizeof(DynamicArray));
  arr->elems = calloc(DYNARR_INITSIZE, elemSize);
  arr->capacity = DYNARR_INITSIZE;
  arr->size = 0;
  arr->elemDestructorFunc = elemDestructorFunc;
  return arr;
}

void DYNARR_destroyArray(DynamicArray *arr)
{
  for(int i = 0; i < arr->size; ++i){
    arr->elemDestructorFunc(arr->elems[i]);
  }
  free(arr->elems);
  free(arr);
}

/* Returns pointer to new array. Initialises the new part of the array with NULLs.
 * If reallocating fails, returns pointer to original array.
 */
void DYNARR_resizeArr(DynamicArray *arr, size_t newCapacity)
{
  void *tempPtr = realloc(arr->elems, newCapacity);
  if(tempPtr != NULL){
    arr->elems = tempPtr;
    arr->capacity = newCapacity;
  }
  for(int i = arr->size; i < newCapacity; ++i)
  {
    arr->elems[i] = NULL;
  }
}

void DYNARR_addElem(DynamicArray *arr, void *elem)
{
  if(arr->size == arr->capacity)
  {
    DYNARR_resizeArr(arr, 2*arr->size);
  }
  arr->elems[arr->size] = elem;
  ++arr->size;
}
