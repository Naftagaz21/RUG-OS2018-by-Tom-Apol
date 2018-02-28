/* Authors: Tom Apol (s2701650), Oanca Andrei Madalin (s3542505) */

#include <stdlib.h> //for good measure
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "DynamicArray.h"

#define _DUP_MAXDIRBUFFERSIZE 256

//destructor function for use with DYNARR_destroyArray.
void destroyString(void *string)
{
  free(string);
}

void getFileNamesInDirRec(DynamicArray *arr, char *dirPath)
{
  DIR* dirPtr;
  struct dirent* entry;

  //open directory
  dirPtr = opendir(dirPath);
  if(dirPtr == NULL) //error handling
  {
    perror("opendir");
    DYNARR_destroyArray(arr); //cleanup
    exit(EXIT_FAILURE);
  }

  //
  char fileName[_DUP_MAXDIRBUFFERSIZE], fileNameBackup[_DUP_MAXDIRBUFFERSIZE];
  char* allocedFileName;
  struct stat fileStatus;

  strcpy(fileNameBackup, dirPath);
  strcat(fileNameBackup, "/");

  //read all entries in this directory
  while((entry = readdir(dirPtr)) != NULL)
  {
    //copy from backup and append file name to the path
    strcpy(fileName, fileNameBackup);
    strcat(fileName, entry->d_name);

    if(stat(fileName, &fileStatus) < 0) //error handling
    {
      perror("stat");
      DYNARR_destroyArray(arr); //cleanup
      exit(EXIT_FAILURE);
    }

    //if we have a regular file: store it
    if(S_ISREG(fileStatus.st_mode))
    {
      //allocate actual memory for the name and store it
      fileNameSpace = malloc(_DUP_MAXDIRBUFFERSIZE * sizeof(char));
      strcpy(fileNameSpace, fileName);

      DYNARR_addElem(arr, fileNameSpace);

    } //if we have a directory: recursively store all regular files
    else if(S_ISDIR(fileStatus.st_mode) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
    {
      getFileNamesInDirRec(arr, fileName);
    }
  }
  closedir(dirPtr);
}

//returns an array of strings filled with all regular file names in the specified directory name
DynamicArray * getFileNamesInDir(char *dirName)
{
  DynamicArray *arr = DYNARR_makeNewArray(sizeof(char *), &destroyString);
  getFileNamesInDirRec(arr, dirName);
  return arr;
}

//compares two files based on size and content. returns false if any path is NULL.
bool compareFiles(const char* filePath1, const char* filePath2)
{
  if(filePath1 == NULL || filePath2 == NULL) return false;

  struct stat fileStatus1;
  struct stat fileStatus2;

  if(stat(filePath1, &fileStatus1) < 0)
  {
    perror("stat, filePath1:");
    exit(EXIT_FAILURE);
  }
  if(stat(filePath2, &fileStatus2) < 0)
  {
    perror("stat, filePath2:");
    exit(EXIT_FAILURE);
  }

  if(fileStatus1.st_size != fileStatus2.st_size)
    return false;

  int fd1 = open(filePath1, O_RDONLY);
  if(fd1 < 0) //error handling
  {
    perror("open");
    exit(EXIT_FAILURE);
  }
  int fd2 = open(filePath2, O_RDONLY);
  if(fd2 < 0) //error handling
  {
    perror("open");
    exit(EXIT_FAILURE);
  }

  char byte1, byte2;
  for(int i = 0; i < fileStatus1.st_size; ++i)
  {
    if(read(fd1, &byte1, 1) < 0)
    {
      close(fd1);
      close(fd2);
      perror("read");
      exit(EXIT_FAILURE);
    }
    if(read(fd2, &byte2, 1) < 0)
    {
      close(fd1);
      close(fd2);
      perror("read");
      exit(EXIT_FAILURE);
    }

    if(byte1 != byte2) return false;
  }

  close(fd1);
  close(fd2);
  return true;
}

//prints out the unique duplicate file pairs in the array.
void printDuplicateFilePairs(DynamicArray *arr, FILE *outStream)
{
  for(int i = 0; i < arr->size; ++i)
  {
    for(int j = i+1; j < arr->size; ++j)
    {
      if(compareFiles(arr->elems[i], arr->elems[j]))
        fprintf(outStream, "%s and %s are the same file\n", (char*)arr->elems[i], (char*)arr->elems[j]);
    }
  }
}


int main(int argc, char* argv[])
{
  /* get list of files in current dir (without dirs as files; recursively) */
  char dirName[_DUP_MAXDIRBUFFERSIZE];
  getcwd(dirName, _DUP_MAXDIRBUFFERSIZE); //get current dir
  //store file names
  DynamicArray *fileNames = getFileNamesInDir(dirName);

  //search and print duplicate pairs
  printDuplicateFilePairs(fileNames, stdout);

  //cleanup
  DYNARR_destroyArray(fileNames);

  return EXIT_SUCCESS;
}
