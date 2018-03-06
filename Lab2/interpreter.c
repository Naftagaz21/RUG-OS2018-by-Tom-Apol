#include <stdio.h>

//redirects the open source file descriptor to the open destination file descriptor.
//src_file_descriptor will be closed.
//returns 0 if all went well. returns -1 if something went wrong, with errno set appropriately
int redirectFilestream(int src_file_descriptor, int dest_file_descriptor)
{
  if(close(dest_file_descriptor) < 0)
    return -1;
  if(dup2(src_file_descriptor, dest_file_descriptor) < 0)
    return -1;
  if(close(src_file_descriptor) < 0)
    return -1;
  return 0;
}

//both file_descriptor and stdin should be open.
//returns 0 if all went well. returns -1 if something went wrong, with errno set appropriately
int redirectToSTDIN(int file_descriptor)
{
  return redirectFilestream(file_descriptor, stdin);
}

//both file_descriptor and stdout should be open.
//returns 0 if all went well. returns -1 if something went wrong, with errno set appropriately
int redirectToSTDOUT(int file_descriptor)
{
  return redirectFilestream(file_descriptor, stdout);
}



//TODO: after redirecting something, make sure all file-descriptors are closed afterwards.

//we assume here that interpretRedirect is only called when the current token is the start of a valid redirection.
//returns 0 if all went well. returns -1 if something went wrong, with errno set appropriately
int interpretRedirect(char **tokens, size_t *index_ptr)
{
  char *word = tokens[*index_ptr + 1];
  int fd;
  if(strcmp(tokens[*index_ptr], "<") == 0)
  {
    if(fd = open(word, O_RDONLY) < 0) //TODO: check whether O_CREAT is necessary here, TODO: check whether the pathname should be prepended with the current directory path
      return -1;

    redirectToSTDIN(fd)
  }
  else if(strcmp(tokens[*index_ptr], ">") == 0)
  {
    if(fd = open(word, O_WRONLY) < 0) //TODO: check whether O_CREAT is necessary here, TODO: check whether the pathname should be prepended with the current directory path
      return -1;
    if(redirectToSTDOUT(fd) < 0)
      return -1;
  }
  *index_ptr += 2;
  return 0;
}

void interpretPipeline(char **tokens, size_t *index_ptr)
{
  //interpret simple_command

  //if there is something after this: interpret pipeline2
}



void interpretTokens(char **tokens)
{
  //interpret pipeline

  //if there is a trailing &, make the resulting process a daemon process.
}
