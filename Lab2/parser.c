//NOTE: This parser assumes all tokens that are not parts of a word are separated by white-spaces.
//      This includes parts of a newline list.
//      Furthermore, it uses the following (probably) LL(1)-grammar for recursive descent parsing.
//      This specification is (currently) slightly different from what the exercise may describe,
//      but we assumed it was not incredibly significant in this course.
//
/*
simple_list
=
pipeline
=
simple_command pipeline2
=
simple_command





*/
// <simple_list> ::= <pipeline>
//                 | <pipeline> '&'
//
// <pipeline> ::= <simple_command>
//              | <simple_command> <pipeline2>
//
// <pipeline2> ::= '|' <pipeline>
//               | '|' <newline_list> <pipeline>
//
// <newline_list> ::= '\\\n'
//                  | <newline_list> '\\\n'
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
// #include <stddef.h>
#include "parser.h"

bool parseRedirection(char **sentence, size_t *current_index_ptr)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
  {
    printf("parseRedirection: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

  if(strcmp(sentence[*current_index_ptr], ">") != 0 && strcmp(sentence[*current_index_ptr], "<") != 0)
  {
    printf("parseRedirection: sentence[%ld] != \"<\" or \">\"\n", *current_index_ptr);
    return false;
  }

  ++(*current_index_ptr);
  if(!parseWord(sentence, current_index_ptr))
  {
    *current_index_ptr = current_index_backup;
    printf("parseRedirection: parseWord failed at index %ld\n", *current_index_ptr);
    return false;
  }
  return true;
}

bool parseLetter(char **sentence, size_t *current_index_ptr, size_t *current_letter_index_ptr)
{
  if(sentence[*current_index_ptr] == NULL)
  {
    printf("parseLetter: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

  char letter = sentence[*current_index_ptr][*current_letter_index_ptr];
  if((letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z'))
  {
    ++(*current_letter_index_ptr);
    return true;
  }
  printf("parseLetter: %c is not valid; at index %ld\n", letter, *current_index_ptr);
  return false;
}

bool parseWord2(char **sentence, size_t *current_index_ptr, size_t *current_letter_index_ptr)
{
  // size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
  {
    printf("parseWord2: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

  //can be empty
  if(sentence[*current_index_ptr][*current_letter_index_ptr] == '\0')
  {
    printf("parseWord2: sentence[%ld][%ld] == '\\0'\n", *current_index_ptr, *current_letter_index_ptr);
    return true;
  }

  if(!parseLetter(sentence, current_index_ptr, current_letter_index_ptr))
  {
    switch(sentence[*current_index_ptr][*current_letter_index_ptr])
    {
      case '/' :
        break;
      case '-' :
        break;
      case '_' :
        break;
      case '.' :
        break;
      default :
        printf("parseWord2: letter = %c, which is invalid; at index %ld\n", sentence[*current_index_ptr][*current_letter_index_ptr], *current_index_ptr);
        return false;
    }
    ++(*current_letter_index_ptr);
  }
  if(!parseWord2(sentence, current_index_ptr, current_letter_index_ptr))
  {
    // *current_index_ptr = current_index_backup;
    // printf("parseWord2: parseWord2 (rec) failed at index %ld\n", *current_index_ptr);
    // return false;
  }
  return true;
}

bool parseWord(char **sentence, size_t *current_index_ptr)
{
  if(sentence[*current_index_ptr] == NULL)
  {
    printf("parseWord: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

  size_t current_letter_index = 0;
  if(!parseLetter(sentence, current_index_ptr, &current_letter_index))
  {
    printf("parseWord: parseLetter failed at index %ld\n", *current_index_ptr);
    return false;
  }


  if(sentence[*current_index_ptr][current_letter_index] != '\0')
    parseWord2(sentence, current_index_ptr, &current_letter_index);
  ++(*current_index_ptr);
  return true;
}

bool parseSimpleCommandElement(char **sentence, size_t *current_index_ptr)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
  {
    printf("parseSimpleCommand: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

  if(!parseWord(sentence, current_index_ptr))
  {
    *current_index_ptr = current_index_backup;
    if(!parseRedirection(sentence, current_index_ptr))
    {
      *current_index_ptr = current_index_backup;
      printf("parseSimpleCommand: parseWord and parseRedirection failed at index %ld\n", *current_index_ptr);
      return false;
    }
  }
  return true;
}

bool parseSimpleCommand(char **sentence, size_t *current_index_ptr)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL){
    printf("parseSimpleCommand: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

  if(!parseSimpleCommandElement(sentence, current_index_ptr))
  {
    *current_index_ptr = current_index_backup;
    printf("parseSimpleCommand: parseSimpleCommandElement failed at index %ld\n", *current_index_ptr);
    return false;
  }

  if(!parseSimpleCommand(sentence, current_index_ptr))
  {
    // *current_index_ptr = current_index_backup;
    printf("parseSimpleCommand: parseSimpleCommand (rec) failed at index %ld\n", *current_index_ptr);
  }
  return true;
}

bool parseNewlineList(char **sentence, size_t *current_index_ptr)
{
  // size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL){
    printf("parseNewlineList: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

  if(strcmp(sentence[*current_index_ptr], "\\\n") != 0) //TODO: check if this should be "\n" instead.
  {
    printf("parseNewlineList: sentence[%ld] != \\n\n", *current_index_ptr);
    return false;
  }
  ++(*current_index_ptr);
  // current_index_backup = *current_index_ptr;
  //TODO: do we need to reset if this returns false?
  parseNewlineList(sentence, current_index_ptr);
  return true;
}

bool parsePipeLine2(char **sentence, size_t *current_index_ptr)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
  {
    printf("parsePipeLine2: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

  if(strcmp(sentence[*current_index_ptr], "|") != 0)
  {
    printf("parsePipeLine2: sentence != \"|\" at index %ld\n", *current_index_ptr);
    return false;
  }
  ++(*current_index_ptr);
  parseNewlineList(sentence, current_index_ptr);
  current_index_backup = *current_index_ptr;
  if(!parsePipeLine(sentence, current_index_ptr))
  {
    *current_index_ptr = current_index_backup;
    printf("parsePipeLine2: parsePipeLine failed at index %ld\n", *current_index_ptr);
    return false;
  }
  return true;
}


//updates current_index to the index after the part of the sentence it has just parsed
bool parsePipeLine(char **sentence, size_t *current_index_ptr)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
  {
    printf("parsePipeLine: sentence[%ld] == NULL\n", *current_index_ptr);
    return false;
  }

  if(!parseSimpleCommand(sentence, current_index_ptr)){
    *current_index_ptr = current_index_backup;
    printf("parsePipeLine: parseSimpleCommand failed at index %ld\n", *current_index_ptr);
    return false;
  }

  current_index_backup = *current_index_ptr;

  if(!parsePipeLine2(sentence, current_index_ptr))
    *current_index_ptr = current_index_backup;
  printf("parsePipeLine returns true\n");
  return true;
}

//sentence array should end with a NULL pointer.
bool parseInput(char **sentence)
{
  size_t currentIndex = 0;
  if(sentence[currentIndex] == NULL){
    printf("sentence[%ld] == NULL\n", currentIndex);
    return false;
  }

  if(!parsePipeLine(sentence, &currentIndex))
  {
    printf("parseInput: parsePipeLine failed at index %ld\n", currentIndex);
    return false;
  }

  printf("currentIndex = %ld\n", currentIndex);
  if(sentence[currentIndex] != NULL && strcmp(sentence[currentIndex], "&") == 0)
  {
    printf("parseInput: sentence[currentIndex] == & :+1:\n");
    ++currentIndex;
  }

  //check whether the expression is finished
  if(sentence[currentIndex] != NULL)
  {
    printf("sentence[%ld] != NULL, as it is %s\n", currentIndex, sentence[currentIndex]);
    return false;
  }

  return true;
}
