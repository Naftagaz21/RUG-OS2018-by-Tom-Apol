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

#define _DUP_MAXDIRBUFFERSIZE 256

int countNrOfFilesInDir(const char* dirPath)
{
  DIR* dirPtr;
  struct dirent* entry;
  int fileCnt = 0;

  dirPtr = opendir(dirPath);
  if(dirPtr == NULL) //error handling
  {
    perror("opendir");
    exit(EXIT_FAILURE);
  }

  char dirPathTotal[_DUP_MAXDIRBUFFERSIZE];
  struct stat fileStatus;

  strcpy(dirPathTotal, dirPath);
  strcat(dirPathTotal, "/");

  char fileName[_DUP_MAXDIRBUFFERSIZE];
  while((entry = readdir(dirPtr)) != NULL)
  {
    strcpy(fileName, dirPathTotal);

    strcat(fileName, entry->d_name);
    if(stat(fileName, &fileStatus) < 0)
    {
      perror("stat");
      exit(EXIT_FAILURE);
    }

    if(S_ISREG(fileStatus.st_mode))
    {
      fileCnt++;
    }
    else if(S_ISDIR(fileStatus.st_mode) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
    {
      fileCnt += countNrOfFilesInDir(fileName);
    }
  }
  closedir(dirPtr);
  return fileCnt;
}



//returns updated indexOffset
int getFileNamesInDirRec(char** fileNames, char* dirPath, int indexOffset)
{
  DIR* dirPtr;
  struct dirent* entry;

  dirPtr = opendir(dirPath);
  if(dirPtr == NULL) //error handling
  {
    perror("opendir");
    exit(EXIT_FAILURE);
  }

  char dirPathTotal[_DUP_MAXDIRBUFFERSIZE];
  struct stat fileStatus;
  strcpy(dirPathTotal, dirPath);
  strcat(dirPathTotal, "/");

  char fileName[_DUP_MAXDIRBUFFERSIZE];

  while((entry = readdir(dirPtr)) != NULL)
  {
    strcpy(fileName, dirPathTotal);
    strcat(fileName, entry->d_name);

    if(stat(fileName, &fileStatus) < 0)
    {
      perror("stat");
      exit(EXIT_FAILURE);
    }

    if(S_ISREG(fileStatus.st_mode))
    {
      strcpy(fileNames[indexOffset], fileName);
      ++indexOffset;
    }
    else if(S_ISDIR(fileStatus.st_mode) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
    {
      indexOffset = getFileNamesInDirRec(fileNames, fileName, indexOffset); //update indexOffset
    }
  }
  closedir(dirPtr);
  return indexOffset;
}


void getFileNamesInDir(char** fileNames, char* dirName)
{
  getFileNamesInDirRec(fileNames, dirName, 0);
}


char** allocFileNames(int fileCount)
{
  char** fileNames = malloc((fileCount+1)*sizeof(char*)); //allocate +1 for terminating NULL pointer
  if(fileNames == NULL)
  {
    perror("malloc"); //hope error value is set if malloc fails?
    exit(EXIT_FAILURE);
  }

  fileNames[0] = malloc(fileCount * _DUP_MAXDIRBUFFERSIZE * sizeof(char));
  if(fileNames[0] == NULL)
  {
    free(fileNames);
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  for(int i = 1; i < fileCount; ++i)
  {
    fileNames[i] = &fileNames[0][i*_DUP_MAXDIRBUFFERSIZE];
  }
  fileNames[fileCount] = NULL;

  return fileNames;
}


void freeFileNames(char** fileNames)
{
  free(fileNames[0]);
  free(fileNames);
}


bool compareFiles(const char* filePath1, const char* filePath2)
{
  //TODO: throw error instead of false
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
  if(fd1 < 0)
  {
    perror("open");
    exit(EXIT_FAILURE);
  }
  int fd2 = open(filePath2, O_RDONLY);
  if(fd2 < 0)
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

//does this in O(n^2), while if we use sorting we can do this in O(n log(n))
void printDuplicateFilePairs(char** fileNames, FILE* outStream)
{
  int i = 0, j = i+1;
  while(fileNames[i] != NULL)
  {
    while(fileNames[j] != NULL)
    {
      if(compareFiles(fileNames[i], fileNames[j]))
      fprintf(outStream, "%s and %s are the same file\n", fileNames[i], fileNames[j]);
      ++j;
    }
    ++i;
  }
}


int main(int argc, char* argv[])
{
  /* get list of files in current dir (without dirs as files; recursively) */
  char dirName[_DUP_MAXDIRBUFFERSIZE];
  getcwd(dirName, _DUP_MAXDIRBUFFERSIZE);
  int fileCount = countNrOfFilesInDir(dirName);
  if(fileCount < 2) return EXIT_SUCCESS;

  // fprintf(stdout, "current file count in working dir = %d\n", fileCount);
  char** fileNames = allocFileNames(fileCount);
  getFileNamesInDir(fileNames, dirName);

  // for(int i = 0; i < fileCount; ++i)
  // {
  //   fprintf(stdout, "Filename[%d]: %s\n", i, fileNames[i]);
  // }

  printDuplicateFilePairs(fileNames, stdout);
  freeFileNames(fileNames);
  return EXIT_SUCCESS;
}
