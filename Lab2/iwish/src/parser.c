//NOTE: This parser assumes all tokens that are not parts of a word are separated by white-spaces.
//      This excludes parts of a newline list, as they are filtered out earlier by the readInput function.
//      Furthermore, it uses the following (probably) LL(1)-grammar for recursive descent parsing.
//      This specification is (currently) slightly different from what the exercise may describe,
//      but we assumed it was not incredibly significant in this course.
//
// <simple_list> ::= <pipeline>
//                 | <pipeline> '&'
//
// <pipeline> ::= <simple_command>
//              | <simple_command> '|' <pipeline>
////            | <simple_command> <pipeline2>
////
//// <pipeline2> ::= '|' <pipeline>
////               | '|' <newline_list> <pipeline>
////
//// <newline_list> ::= '\n'
////                  | <newline_list> '\n'
//
// <simple_command> ::= <simple_command_element>
//                    | <simple_command_element> <simple_command>
//
// //note: first is empty
// //<simple_command2> ::= <simple_command_element> <simple_command2>
//
// <simple_command_element> ::= <word>
//                            | <redirection>
//
// <word> ::= <letter>
//          | <letter> <word2>
//
// <word2> ::= <letter>
//           | '_'
//           | '.'
//           | '/'
//           | '-'
//           | <letter> <word2>
//           | '_' <word2>
//           | '.' <word2>
//           | '/' <word2>
//           | '-' <word2>
//
// <letter> ::= a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|
//              A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z
//
// <redirection> ::= '>' <word>
//                 | '<' <word>



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
  {
    // printf("parseRedirection: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

  if(strcmp(sentence[*current_index_ptr], "<") == 0)
  {
    redirection_ptr->isInputRedirection = 1;
  }
  else if (strcmp(sentence[*current_index_ptr], ">") == 0)
  {
    redirection_ptr->isInputRedirection = 0;
  } else {
    // printf("parseRedirection: sentence[%ld] != \"<\" or \">\"\n", *current_index_ptr);
    return false;
  }

  ++(*current_index_ptr);
  if(!parseWord(sentence, current_index_ptr))
  {
    *current_index_ptr = current_index_backup;
    // printf("parseRedirection: parseWord failed at index %ld\n", *current_index_ptr);
    return false;
  }
  redirection_ptr->word = sentence[*current_index_ptr -1];
  return true;
}


bool parseLetter(char **sentence, size_t *current_index_ptr, size_t *current_letter_index_ptr)
{
  if(sentence[*current_index_ptr] == NULL)
  {
    // printf("parseLetter: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

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
  // printf("parseLetter: %c is not valid; at index %ld\n", letter, *current_index_ptr);
  return false;
}


bool parseWord(char **sentence, size_t *current_index_ptr)
{
  if(sentence[*current_index_ptr] == NULL)
  {
    // printf("parseWord: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

  size_t current_letter_index = 0;
  while(sentence[*current_index_ptr][current_letter_index] != '\0')
  {
    if(!parseLetter(sentence, current_index_ptr, &current_letter_index))
    {
      // printf("parseWord: parseLetter failed at index %ld and letter_index %ld\n", *current_index_ptr, current_letter_index);
      return false;
    }
  }

  ++(*current_index_ptr);
  return true;
}


bool parseSimpleCommandElement(char **sentence, size_t *current_index_ptr, Simple_Command_Element *element_ptr)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
  {
    // printf("parseSimpleCommand: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

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
      // printf("parseSimpleCommand: parseWord and parseRedirection failed at index %ld\n", *current_index_ptr);
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
  if(sentence[*current_index_ptr] == NULL){
    // printf("parseSimpleCommand: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

  if(!parseSimpleCommandElement(sentence, current_index_ptr, &(element_list_ptr->element)))
  {
    *current_index_ptr = current_index_backup;
    // printf("parseSimpleCommand: parseSimpleCommandElement failed at index %ld\n", *current_index_ptr);
    return false;
  }

  if((element_list_ptr->next = makeNewSimpleCommandElementList()) == NULL)
    return false; //TODO: check whether we need to let the outside world know that this fucked up.

  if(!parseSimpleCommand(sentence, current_index_ptr, element_list_ptr->next))
  {
    // *current_index_ptr = current_index_backup;
    // printf("parseSimpleCommand: parseSimpleCommand (rec) failed at index %ld\n", *current_index_ptr);
    freeSimpleCommandElementList(element_list_ptr->next);
    element_list_ptr->next = NULL;
  }

  return true;
}


bool parsePipeLine2(char **sentence, size_t *current_index_ptr, Simple_Command_List * previous_command_list)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
  {
    // printf("parsePipeLine2: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

  if(strcmp(sentence[*current_index_ptr], "|") != 0)
  {
    // printf("parsePipeLine2: sentence != \"|\" at index %ld\n", *current_index_ptr);
    return false;
  }
  ++(*current_index_ptr);
  current_index_backup = *current_index_ptr;

  //allocate space for the upcoming pipeline token
  if((previous_command_list->next = makeNewSimpleCommandList()) == NULL)
    return false;


  if(!parsePipeLine(sentence, current_index_ptr, previous_command_list->next))
  {
    //reset everything to up to this point / cleanup.
    *current_index_ptr = current_index_backup;
    // printf("parsePipeLine2: parsePipeLine failed at index %ld\n", *current_index_ptr);
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
  {
    // printf("parsePipeLine: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

  if(!parseWord(sentence, current_index_ptr))
  {
    fprintf(stderr, "error: a command must start with a program path. error at token %s\n", sentence[*current_index_ptr]);
    return false;
  }
  else
    --*current_index_ptr;

  if(!parseSimpleCommand(sentence, current_index_ptr, (simple_command_list_ptr->simple_command).element_list)){
    *current_index_ptr = current_index_backup;
    // printf("parsePipeLine: parseSimpleCommand failed at index %ld\n", *current_index_ptr);
    return false;
  }

  current_index_backup = *current_index_ptr;

  if(!parsePipeLine2(sentence, current_index_ptr, simple_command_list_ptr))
    *current_index_ptr = current_index_backup;
  // printf("parsePipeLine returns true\n");
  return true;
}


//sentence array should end with a NULL pointer.
//takes a pointer to an empty Simple_List and fills it according to what it parses.
//if parseInput fails (i.e. returns false), the contents of the Simple_List are undefined.
bool parseInput(char **sentence, Simple_List *simple_list_ptr)
{
  size_t currentIndex = 0;
  if(sentence[currentIndex] == NULL){
    // printf("sentence[%ld] == NULL\n", currentIndex);
    return false;
  }

  if(!parsePipeLine(sentence, &currentIndex, (simple_list_ptr->pipeline).simple_command_list))
  {
    // printf("parseInput: parsePipeLine failed at index %ld\n", currentIndex);
    return false;
  }

  // printf("currentIndex = %ld\n", currentIndex);
  if(sentence[currentIndex] != NULL && strcmp(sentence[currentIndex], "&") == 0)
  {
    // printf("parseInput: sentence[currentIndex] == & :+1:\n");
    simple_list_ptr->hasDaemonAmpersand = 1;
    ++currentIndex;
  }

  //check whether the expression is finished
  if(sentence[currentIndex] != NULL)
  {
    // printf("sentence[%ld] != NULL, as it is %s\n", currentIndex, sentence[currentIndex]);
    return false;
  }

  return true;
}
