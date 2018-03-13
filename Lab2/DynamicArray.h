#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

//elements are void*'s
#define DYNARR_INITSIZE 16

typedef struct DynamicArray {
  void** elems;
  size_t size;
  size_t capacity;
  void (*elemDestructorFunc)(void *elem);
} DynamicArray;

//returns NULL if allocation failed
DynamicArray * DYNARR_makeNewArray(size_t elemSize, void(*elemDestructorFunc)(void *elem));

/* Frees array and calls the elementDestructorFunction that was passed at its creation to free its elements.
 * if elementDestructorFunction == NULL, individual elements are not freed (useful in case the elements are located on the stack)
 * if internalArrayPersistence == 1, the internal array is not freed and a pointer towards it is returned.
 *   else the entire array is freed and NULL is returned.
 */
void * DYNARR_destroyArray(DynamicArray *arr, int internalArrayPersistence);

/* Resizes the array to a particular size.
 * When successful, returns 0.
 * On failure, leaves the array intact and returns -1.
 */
int DYNARR_resizeArr(DynamicArray *arr, size_t newCapacity);

/* Adds an element to the array and resizes the array if necessary.
 * When successful, returns 0.
 * On failure, leaves the array intact and returns -1.
 */
int DYNARR_addElem(DynamicArray *arr, void *elem);

/* Appends the elements of arr2 to arr1.
 * Note that these are only references to the elements in arr2.
 *  if DYNARR_destroyArray(arr2, 0) is called, the elements of arr2 in arr1
 *  are destroyed as well, without arr1 being aware of this, which may lead to
 *  double frees when DYNARR_destroyArray(arr1, 0) is called afterwards.
 * On success returns 0, on failure returns -1.
 * On failure, arr1 will remain unchanged.
 */
int DYNARR_concatArrs(DynamicArray *arr1, DynamicArray *arr2);

#endif
