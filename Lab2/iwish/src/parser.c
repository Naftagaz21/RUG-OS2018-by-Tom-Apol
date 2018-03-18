
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "parser.h"
#include "parsertokens.h"

bool parseRedirection(char **sentence, size_t *current_index_ptr, Redirection *redirection_ptr)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
    return false;

  if(strcmp(sentence[*current_index_ptr], "<") == 0)
    redirection_ptr->isInputRedirection = 1;
  else if (strcmp(sentence[*current_index_ptr], ">") == 0)
    redirection_ptr->isInputRedirection = 0;
  else
    return false;

  ++(*current_index_ptr);
  if(!parseWord(sentence, current_index_ptr))
  {
    *current_index_ptr = current_index_backup;
    return false;
  }
  redirection_ptr->word = sentence[*current_index_ptr -1];
  return true;
}


bool parseLetter(char **sentence, size_t *current_index_ptr, size_t *current_letter_index_ptr)
{
  if(sentence[*current_index_ptr] == NULL)
    return false;

  char letter = sentence[*current_index_ptr][*current_letter_index_ptr];
  if((letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z'))
  {
    ++(*current_letter_index_ptr);
    return true;
  }
  else if(letter == '-' || letter == '.' || letter == '/' || letter == '_')
  {
    ++(*current_letter_index_ptr);
    return true;
  }
  return false;
}


bool parseWord(char **sentence, size_t *current_index_ptr)
{
  if(sentence[*current_index_ptr] == NULL)
    return false;

  size_t current_letter_index = 0;
  while(sentence[*current_index_ptr][current_letter_index] != '\0')
    if(!parseLetter(sentence, current_index_ptr, &current_letter_index))
      return false;

  ++(*current_index_ptr);
  return true;
}


bool parseSimpleCommandElement(char **sentence, size_t *current_index_ptr, Simple_Command_Element *element_ptr)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
    return false;

  if(parseWord(sentence, current_index_ptr))
  {
    element_ptr->isRedirection = 0;
    element_ptr->word = sentence[current_index_backup];
    element_ptr->redirection_pointer = NULL;
  }
  else
  {
    *current_index_ptr = current_index_backup;
    element_ptr->redirection_pointer = makeNewRedirection();
    if(parseRedirection(sentence, current_index_ptr, element_ptr->redirection_pointer))
    {
      element_ptr->isRedirection = 1;
      element_ptr->word = NULL;
    }
    else
    {
      *current_index_ptr = current_index_backup;
      freeRedirection(element_ptr->redirection_pointer);
      element_ptr->redirection_pointer = NULL;
      return false;
    }
  }
  return true;
}


bool parseSimpleCommand(char **sentence, size_t *current_index_ptr, Simple_Command_Element_List *element_list_ptr)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
    return false;

  if(!parseSimpleCommandElement(sentence, current_index_ptr, &(element_list_ptr->element)))
  {
    *current_index_ptr = current_index_backup;
    return false;
  }

  if((element_list_ptr->next = makeNewSimpleCommandElementList()) == NULL)
    return false;

  if(!parseSimpleCommand(sentence, current_index_ptr, element_list_ptr->next))
  {
    freeSimpleCommandElementList(element_list_ptr->next);
    element_list_ptr->next = NULL;
  }

  return true;
}


bool parsePipeLine2(char **sentence, size_t *current_index_ptr, Simple_Command_List * previous_command_list)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
    return false;

  if(strcmp(sentence[*current_index_ptr], "|") != 0)
    return false;

  ++(*current_index_ptr);
  current_index_backup = *current_index_ptr;

  //allocate space for the upcoming pipeline token
  if((previous_command_list->next = makeNewSimpleCommandList()) == NULL)
    return false;

  if(!parsePipeLine(sentence, current_index_ptr, previous_command_list->next))
  {
    //reset everything to up to this point / cleanup.
    *current_index_ptr = current_index_backup;
    freeSimpleCommandList(previous_command_list->next);
    previous_command_list->next = NULL;
    return false;
  }
  return true;
}


//updates current_index to the index after the part of the sentence it has just parsed
bool parsePipeLine(char **sentence, size_t *current_index_ptr, Simple_Command_List *simple_command_list_ptr)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
    return false;

  if(!parseWord(sentence, current_index_ptr))
  {
    fprintf(stderr, "error: a command must start with a program path. error at token %s\n", sentence[*current_index_ptr]);
    return false;
  }
  else
    --*current_index_ptr;

  if(!parseSimpleCommand(sentence, current_index_ptr, (simple_command_list_ptr->simple_command).element_list)){
    *current_index_ptr = current_index_backup;
    return false;
  }

  current_index_backup = *current_index_ptr;

  if(!parsePipeLine2(sentence, current_index_ptr, simple_command_list_ptr))
    *current_index_ptr = current_index_backup;
  return true;
}


//sentence array should end with a NULL pointer.
//takes a pointer to an empty Simple_List and fills it according to what it parses.
//if parseInput fails (i.e. returns false), the contents of the Simple_List are undefined,
// but a call to freeSimpleList will still fully free the datastructure.
bool parseInput(char **sentence, Simple_List *simple_list_ptr)
{
  size_t currentIndex = 0;
  if(sentence[currentIndex] == NULL)
    return false;

  if(!parsePipeLine(sentence, &currentIndex, (simple_list_ptr->pipeline).simple_command_list))
    return false;

  if(sentence[currentIndex] != NULL && strcmp(sentence[currentIndex], "&") == 0)
  {
    simple_list_ptr->hasDaemonAmpersand = 1;
    ++currentIndex;
  }

  //check whether the expression is finished
  return sentence[currentIndex] == NULL ? true : false;
}
