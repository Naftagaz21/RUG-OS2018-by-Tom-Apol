#ifndef SAFELYFORK_H
#define SAFELYFORK_H

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/prctl.h>

/* Forks a process and makes sure that when the parent terminates,
 * the child will terminate as well.
 * Returns a valid child process ID (similar to fork(2)) and -1 on failure.
 * Prints relevant error messages to stderr.
 */
int safelyFork();

#endif
