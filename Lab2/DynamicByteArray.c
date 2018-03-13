//WIP!



elems = char *bytestring;

size_t type_size = <size_of_type>;





void addElemToArr(DynArr arr, void *bytestring)
{
  //TODO: resize if needed

  for(int i = 0; i < type_size; ++i)
  {
    elems[nrOfElems * type_size + i] = ((char *) bytestring)[i];
  }
}


void * getElem(DynArr *arr, size_t index)
{
  return &(arr->elems[index * arr->type_size]);
}

//usage
// int integer = * ((int *) getElem(arr, index));
