#include <stdlib.h>
#include "DynamicArray.h"

//returns NULL if allocation failed
DynamicArray * DYNARR_makeNewArray(size_t elemSize, void(*elemDestructorFunc)(void *elem))
{
  DynamicArray *arr = malloc(sizeof(DynamicArray));
  arr->elems = malloc(DYNARR_INITSIZE * elemSize);
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

//returns pointer to new array. If reallocating fails, returns pointer to original array.
void DYNARR_resizeArr(DynamicArray *arr, size_t newCapacity)
{
  void *tempPtr = realloc(arr->elems, newCapacity);
  if(tempPtr != NULL){
    arr->elems = tempPtr;
    arr->capacity = newCapacity;
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
