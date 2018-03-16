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
    for(int i = 0; i < arr->size; ++i)
    {
      arr->elemDestructorFunc(arr->elems[i]);
    }
  free(arr->elems);
  free(arr);
  return NULL;
}

/* When successful, returns 0.
 * newCapacity expects a number of elements, not a number of bytes.
 * On failure, leaves the array intact and returns -1.
 */
int DYNARR_resizeArr(DynamicArray *arr, size_t newCapacity)
{
  void *tempPtr = realloc(arr->elems, newCapacity*sizeof(void*));
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

/* Appends the elements of arr2 to arr1.
 * Note that these are only references to the elements in arr2.
 *  if DYNARR_destroyArray(arr2, 0) is called, the elements of arr2 in arr1
 *  are destroyed as well, without arr1 being aware of this, which may lead to
 *  double frees when DYNARR_destroyArray(arr1, 0) is called afterwards.
 * On success returns 0, on failure returns -1.
 * On failure, arr1 will remain unchanged.
 */
int DYNARR_concatArrs(DynamicArray *arr1, DynamicArray *arr2)
{
  size_t arr1_size_backup = arr1->size;
  size_t arr1_capacity_backup = arr1->capacity;

  int i = 0;
  void *element;
  while(i < arr2->size)
  {
    element = arr2->elems[i];
    if(DYNARR_addElem(arr1, element) < 0)
    {
      DYNARR_resizeArr(arr1, arr1_capacity_backup);
      arr1->size = arr1_size_backup;
      return -1;
    }
  }
  return 0;
}
