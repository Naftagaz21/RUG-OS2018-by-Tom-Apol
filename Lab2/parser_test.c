
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

int main(int argc, char *argv[])
{
  if(argc != 2)
  {
    fprintf(stderr, "argc should be 2, while argc = %d\n", argc);
    exit(EXIT_FAILURE);
  }

  char* argv1_copy = malloc((strlen(argv[1])+1)*sizeof(char));
  strcpy(argv1_copy, argv[1]);

  size_t tokenAmnt = 0;
  char* char_ptr = strtok(argv1_copy, " ");
  while(char_ptr != NULL)
  {
    ++tokenAmnt;
    char_ptr = strtok(NULL, " ");
  }

  if(tokenAmnt == 0)
  {
    fprintf(stderr, "no tokens to parse\n");
    exit(EXIT_FAILURE);
  }

  strcpy(argv1_copy, argv[1]);
  char **strings = malloc((tokenAmnt+1) * sizeof(char*));
  char_ptr = strtok(argv1_copy, " ");
  size_t i = 0;
  while(char_ptr != NULL)
  {
    strings[i] = malloc(strlen(char_ptr)+1);
    strcpy(strings[i], char_ptr);
    ++i;
  }
  strings[i] = NULL;

  fprintf(stdout, "\"%s\" is valid: %s\n", argv[1], parseInput(strings) ? "true" : "false");

  free(argv1_copy);
  for(size_t i = 0; i < tokenAmnt; ++i)
  {
    free(strings[i]);
  }
  free(strings);

  return 0;
}
