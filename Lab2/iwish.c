/* Battleplan:
 * 1) Use strtok to split the string into tokens. Put the tokens in a list.
 * 2) Write a parser to interpret the token list.
 * 3)
 */


//IMPORTANT NOTE: For now this program only handles the lower half of ASCII, as I cannot be bothered to try and learn how to handle UTF-8 in C on such short notice.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bool.h>
#include <errno.h>
#include "DynamicArray.h"

/* ======== DESTRUCTORS ======== */
//destructor function for use with DYNARR_destroyArray.
void destroyString(void *string)
{
  free(string);
}

//destructor function for use with DYNARR_destroyArray.
void destroyChar(void *ch)
{
  free(ch);
}

/* ======== I/O ======== */

//Returns a DynamicArray of characters, terminated by a \0 character.
DynamicArray * readInput(FILE *fileStream)
{
  DynamicArray arr = DYNARR_makeNewArray(sizeof(char), &destroyChar);
  char ch;
  char *chptr;

  do
  {
    //we assume only values in the range of [0, 128)
    c = (char) fgetc(fileStream);
    chptr = malloc(sizeof(char));

    if(chptr == NULL)
    {
      perror("readInput: malloc");
      DYNARR_destroyArray(&arr);
      exit(EXIT_FAILURE);
    }

    *chptr = ch;
    DYNARR_addElem(&arr, chptr);
  }
  while (ch != EOF);

  //add terminating 0 character.
  chptr = calloc(1, sizeof(char));
  if(chptr == NULL)
  {
    perror("readInput: malloc");
    DYNARR_destroyArray(&arr);
    exit(EXIT_FAILURE);
  }
  DYNARR_addElem(chptr);

  return &arr;
}

/* ======== PARSING ======== */
/* Tokenise the input string, such that we can parse it.
 * Returns a DynamicArray of strings.
 */
DynamicArray * tokenise(const char *string)
{
  char *backupString = malloc(strlen(string)+1, sizeof(char));
  strcpy(backupString, string);

  DynamicArray tokenArr = DYNARR_makeNewArray(sizeof(char*), &destroyString);

  //fill tokenArr with tokens
  char* tokenBuf = strtok(string, " ");
  char* token;
  while(tokenBuf != NULL)
  {
    token = malloc(strlen(tokenBuf)+1 * sizeof(char));
    strcpy(token, tokenBuf);
    DYNARR_addElem(tokenArr, token);
    tokenBuf = strtok(NULL, " ");
  }
  return &tokenArr;
}


int main(int argc, char* argv[])
{
  int status;
  while(true)
  {

  }


  return EXIT_SUCCESS;
}
