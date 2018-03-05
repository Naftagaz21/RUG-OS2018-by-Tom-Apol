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
// // <simple_command> ::= <simple_command_element> <simple_command>
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
// <word2> ::= <letter> <word2>
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
// #include <stddef.h>
#include "parser.h"

bool parseRedirection(char **sentence, size_t *current_index_ptr)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
    return false;

  if(strcmp(sentence[*current_index_ptr], ">") != 0 && strcmp(sentence[*current_index_ptr], "<") != 0)
    return false;

  ++(*current_index_ptr);
  if(!parseWord(sentence, current_index_ptr))
  {
    *current_index_ptr = current_index_backup;
    return false;
  }
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
  return false;
}

bool parseWord2(char **sentence, size_t *current_index_ptr, size_t *current_letter_index_ptr)
{
  if(sentence[*current_index_ptr] == NULL)
  return false;

  if(sentence[*current_index_ptr][*current_letter_index_ptr] == '\0')
    return true;

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
      return false;
    }
    ++(*current_letter_index_ptr);
  }
  return parseWord2(sentence, current_index_ptr, current_letter_index_ptr);
}

bool parseWord(char **sentence, size_t *current_index_ptr)
{
  if(sentence[*current_index_ptr] == NULL)
    return false;

  size_t current_letter_index = 0;
  if(!parseLetter(sentence, current_index_ptr, &current_letter_index))
    return false;


  if(sentence[*current_index_ptr][current_letter_index] != '\0')
    parseWord2(sentence, current_index_ptr, &current_letter_index);
  ++(*current_index_ptr);
  return true;
}

bool parseSimpleCommandElement(char **sentence, size_t *current_index_ptr)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
    return false;

  if(!parseWord(sentence, current_index_ptr))
  {
    *current_index_ptr = current_index_backup;
    if(!parseRedirection(sentence, current_index_ptr))
    {
      *current_index_ptr = current_index_backup;
      return false;
    }
  }
  return true;
}

bool parseSimpleCommand(char **sentence, size_t *current_index_ptr)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
  return false;

  if(!parseSimpleCommandElement(sentence, current_index_ptr))
  {
    *current_index_ptr = current_index_backup;
    return false;
  }

  if(!parseSimpleCommand(sentence, current_index_ptr))
  {
    *current_index_ptr = current_index_backup;
  }
  return true;
}

bool parseNewlineList(char **sentence, size_t *current_index_ptr)
{
  // size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
    return false;
  if(strcmp(sentence[*current_index_ptr], "\\\n") != 0) //TODO: check if this should be "\n" instead.
  {
    return false;
  }
  ++(*current_index_ptr);
  //TODO: do we need to reset if this returns false?
  parseNewlineList(sentence, current_index_ptr);
  return true;
}

bool parsePipeLine2(char **sentence, size_t *current_index_ptr)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
    return false;

  if(strcmp(sentence[*current_index_ptr], "|") != 0)
  {
    return false;
  }
  ++(*current_index_ptr);
  parseNewlineList(sentence, current_index_ptr);

  if(!parsePipeLine(sentence, current_index_ptr))
  {
    *current_index_ptr = current_index_backup;
    return false;
  }
  return true;
}


//updates current_index to the index after the part of the sentence it has just parsed
bool parsePipeLine(char **sentence, size_t *current_index_ptr)
{
  size_t current_index_backup = *current_index_ptr;
  if(sentence[*current_index_ptr] == NULL)
    return false;

  if(!parseSimpleCommand(sentence, current_index_ptr)){
    *current_index_ptr = current_index_backup;
    return false;
  }

  if(!parsePipeLine2(sentence, current_index_ptr))
    *current_index_ptr = current_index_backup;
  return true;
}

//sentence array should end with a NULL pointer.
bool parseInput(char **sentence)
{
  size_t currentIndex = 0;
  if(sentence[currentIndex] == NULL)
    return false;

  if(!parsePipeLine(sentence, &currentIndex))
    return false;

  if(strcmp(sentence[currentIndex], "&") == 0)
    ++currentIndex;

  //check whether the expression is finished
  if(sentence[currentIndex] != NULL)
    return false;

  return true;
}
