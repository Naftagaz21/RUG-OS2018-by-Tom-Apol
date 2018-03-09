#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

//elements are void*'s
#define DYNARR_INITSIZE 16

typedef struct DynamicArray {
  void** elems;
  size_t size;
  size_t capacity;
  int internalArrayPersistence;
  void (*elemDestructorFunc)(void *elem);
} DynamicArray;

//returns NULL if allocation failed
DynamicArray * DYNARR_makeNewArray(size_t elemSize, void(*elemDestructorFunc)(void *elem), int internalArrayPersistence);

//frees array and calls the elementDestructorFunction that was passed at its creation to free its elements.
void DYNARR_destroyArray(DynamicArray *arr);

//returns pointer to new array. If reallocating fails, returns pointer to original array.
void DYNARR_resizeArr(DynamicArray *arr, size_t newCapacity);

void DYNARR_addElem(DynamicArray *arr, void *elem);

#endif
