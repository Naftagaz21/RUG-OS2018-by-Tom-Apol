/* Authors: Tom Apol (s2701650), Oanca Andrei Madalin (s3542505)
 * Program description:
 *       Creates a ring of processes, the amount defined by argv[1],
 *       communicating through unidirectional pipes.
 *       Starting from the oldest process, a number is passed around.
 *       This number is then printed to stdout, incremented and passed to the
 *       next process, until the STOPNUMBER is reached (defined below as 50).
 *       Finally, all processes
 *
 * Note: While recursively creating pipes should be possible,
 *       it seems more clunky and messy than creating all pipes from start.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

#define MINPROC 2
#define MAXPROC 16
#define STOPNUMBER 50
#define MYSIG_WAITFORCHILD -1

/* =========== PIPES =========== */

/* Creates the necessary amount of pipes and stores it in an array. */
int** createPipes(int nrOfProcesses)
{
  /* Create pipes */
  int** pipeArr = malloc(nrOfProcesses * sizeof(int*));
  pipeArr[0] = malloc(nrOfProcesses * 2 * sizeof(int));

  for(int i = 0; i < nrOfProcesses; ++i)
  {
    pipeArr[i] = &(pipeArr[0][2*i]);

    if(pipe(pipeArr[i]) != 0)
    {
      perror("Error in createPipes");
      exit(EXIT_FAILURE);
    }
  }
  return pipeArr;
}

/* Initialises the pipe by closing the unused end of the pipe,
   depending on whether it is an outgoing connection.
 */
void initPipe(int* pipe, bool isOutgoing)
{
  close(pipe[isOutgoing ? 0 : 1]);
}

/* Closes used end of the pipe in preparation for its destruction. */
void closePipe(int* pipe, bool isOutgoing)
{
  close(pipe[isOutgoing ? 1 : 0]);
}

/* Closes both ends of a pipe */
void closeUnnecessaryPipe(int* pipe)
{
  close(pipe[0]);
  close(pipe[1]);
}

/* Frees the pipeArray */
void freePipes(int** pipeArr)
{
  free(pipeArr[0]);
  free(pipeArr);
}


/* =========== FORKING =========== */

/* Recursively forks this process until n processes
 * (including the calling process) have been created.
 * Returns the process number [0, n).
 * Fails and exits when a non-positive n is given.
 */

int recFork(int n, unsigned int processNr)
{
  if(processNr == n-1) return processNr;
  int childID = fork();
  if(childID == -1)
  {
    perror("Error in recFork");
    exit(EXIT_FAILURE);
  }
  return (childID == 0 ? recFork(n, processNr+1) : processNr);
}

unsigned int recursivelyForkToNProcesses(int n)
{
  if(n < 1)
  {
    fprintf(stderr, "Error in forkToNProcesses: non-positive input n = %d\n", n);
    exit(EXIT_FAILURE);
  }

  return recFork(n, 0);
}

/* =========== PROCESS SPECIFIC =========== */

/* retrieves the output pipe of this process from pipeArr */
int* getMyOutputPipe(int** pipeArr, unsigned int myProcessNr)
{
  return pipeArr[myProcessNr];
}

/* retrieves the input pipe of this process from pipeArr */
int* getMyInputPipe(int** pipeArr, unsigned int myProcessNr, int nrOfProcesses)
{
  return pipeArr[(myProcessNr + nrOfProcesses -1)%nrOfProcesses];
}


/* Closes unnecessary inputs/outputs for my pipes in pipeArr
 * and closes all other pipes.
 */
void initMyPipes(int** pipeArr, unsigned int myProcessNr, int nrOfProcesses)
{
  int* myInputPipe = getMyInputPipe(pipeArr, myProcessNr, nrOfProcesses);
  int* myOutputPipe = getMyOutputPipe(pipeArr, myProcessNr);

  initPipe(myOutputPipe, true);
  initPipe(myInputPipe, false);

  //close all other pipes
  for(int i = 0; i < nrOfProcesses; ++i)
  {
    if(pipeArr[i] != myInputPipe && pipeArr[i] != myOutputPipe)
      closeUnnecessaryPipe(pipeArr[i]);
  }
}

/* closes the pipes in use by process <myProcessNr> */
void closeMyPipes(int** pipeArr, unsigned int myProcessNr, int nrOfProcesses)
{
  closePipe(getMyOutputPipe(pipeArr, myProcessNr), true);
  closePipe(getMyInputPipe(pipeArr, myProcessNr, nrOfProcesses), false);
}

/* reads and returns the int from the input pipe of process <myProcessNr> in pipeArr */
int readIntFromMyPipe(int** pipeArr, unsigned int myProcessNr, int nrOfProcesses)
{
  /*TODO: add error handling */
  int tempInt;
  read(getMyInputPipe(pipeArr, myProcessNr, nrOfProcesses)[0], &tempInt, sizeof(int));
  return tempInt;
}

/* writes the given int to the output pipe of process <myProcessNr> in pipeArr */
void writeIntToMyPipe(int** pipeArr, int integer, unsigned int myProcessNr)
{
  /*TODO: add error handling */
  int tempInt = integer;
  write(getMyOutputPipe(pipeArr, myProcessNr)[1], &tempInt, sizeof(int));
}


/* =========== MISC =========== */

void printOutput(FILE* fileptr, int number)
{
  fprintf(fileptr, "pid=%d: %d\n", getpid(), number);
}


/* checks whether a c-string represents a natural number (>= 0) */
/* rejects all strings that contain non-digit characters */
bool isNatNum(const char* string)
{
  if(string == NULL)
  {
    fprintf(stderr, "Error in isNumber: string = NULL\n");
    exit(EXIT_FAILURE);
  }

  char c = string[0];
  for(int i = 0; i <= strlen(string); ++i)
  {
    if(!isdigit(c)) return false;
  }
  return true;
}


/* =========== REFACTORED SECTIONS OF MAIN =========== */

void printUsage(FILE* fileptr)
{
  fprintf(fileptr, "Usage: ring <number of processes>,\
 where 2 <= <number of processes> <= 16.\n");
}


/* Cycle the number through the ring of processes, from 0 until 50.
 * After that, the MYSIG_WAITFORCHILD signal is cycled around to tell every
 * process to stop.
 */
void cycleNumber(int** pipeArr, unsigned int myProcessNr, int nrOfProcesses)
{
  /* Initialiate the cycling with the root process (with processNr 0) */
  int number = 0;

  if(myProcessNr != 0)
  {
    number = readIntFromMyPipe(pipeArr, myProcessNr, nrOfProcesses);
  }

  while(number != MYSIG_WAITFORCHILD)
  {
    printOutput(stdout, number);
    if(number == STOPNUMBER)
      break;
    writeIntToMyPipe(pipeArr, number+1, myProcessNr);

    number = readIntFromMyPipe(pipeArr, myProcessNr, nrOfProcesses);
  }

  /* Signal the next process that it's time to stop */
  writeIntToMyPipe(pipeArr, MYSIG_WAITFORCHILD, myProcessNr);

  /* Ensure that the process that received the STOPNUMBER doesn't termintate
   * before they have received their stop signal, as we don't want their parent
   *  to write to an already closed pipe.
   */
  if(number == STOPNUMBER)
    readIntFromMyPipe(pipeArr, myProcessNr, nrOfProcesses);
}

/* Close all remaining pipes and free the entire pipe array */
void cleanup(int** pipeArr, unsigned int myProcessNr, int nrOfProcesses)
{
  closeMyPipes(pipeArr, myProcessNr, nrOfProcesses);
  freePipes(pipeArr);
}


/* =========== MAIN =========== */

int main (int argc, char* argv[])
{
  /* Validate input */
  int nrOfProcesses;
  if(argc != 2 || !isNatNum(argv[1]))
  {
    printUsage(stderr);
    exit(EXIT_FAILURE);
  } else {
    nrOfProcesses = atoi(argv[1]);

    if(nrOfProcesses < MINPROC || nrOfProcesses > MAXPROC)
    {
      printUsage(stderr);
      exit(EXIT_FAILURE);
    }
  }

  /* Create pipes */
  int** pipeArr = createPipes(nrOfProcesses);

  /* Recursively spawn up to nrOfProcesses children */
  unsigned int myProcessNr = recursivelyForkToNProcesses(nrOfProcesses);

  /* Initiate my pipes */
  initMyPipes(pipeArr, myProcessNr, nrOfProcesses);

  /* Start cycling the number around */
  cycleNumber(pipeArr, myProcessNr, nrOfProcesses);

  /* Clean up all processes recursively, such that no zombie processes remain */
  /* Close all remaining pipes and free them */
  cleanup(pipeArr, myProcessNr, nrOfProcesses);

  int status;
  waitpid(-1, &status, 0); //wait for child to die

  return EXIT_SUCCESS;
}
