/* Authors: Tom Apol (s2701650), Oanca Andrei Madalin (s3542505) */
/* Note for reworking: use a list to store the command and arguments,
 * such that you only need one loop.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "Usage: execute \"<command> <arguments>\"\n");
        exit(EXIT_FAILURE);
    }
    char* command = malloc((strlen(argv[1]) + 1) * sizeof(char*));
    strcpy(command, argv[1]);


    char* token;
    int tokenamount = 0;

    //get amount of tokens for allocation of newargv
    token = strtok(command, " ");
    while(token != NULL)
    {
        ++tokenamount;
        token = strtok(NULL, " ");
    }
    strcpy(command, argv[1]); //for some reason strtok cuts off the end, so copy it again from argv[1]

    //allocate newargv and fill it with commandline arguments
    char** newargv = malloc((tokenamount+1) * sizeof(char*));
    token = strtok(command, " ");
    int i = 0;
    while(token != NULL)
    {
        newargv[i] = malloc((strlen(token) + 1) * sizeof(char));
        strcpy(newargv[i], token);
        token = strtok(NULL, " ");
        i++;
    }
    newargv[i] = NULL; //argv by definition ends with a null-pointer

    execvp(newargv[0], newargv);


    //we will only get here if exec() fails
    fprintf(stderr, "Execute: command %s could not be found\n", newargv[0]);

    //free everything to avoid leakage
    for(i = 0; i < tokenamount; ++i)
    {
        free(newargv[i]);
    }
    free(newargv);
    free(command);

    return EXIT_SUCCESS;
}
