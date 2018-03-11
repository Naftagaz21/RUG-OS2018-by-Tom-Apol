#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stddef.h>
#include "parsertokens.h"

bool parseInput(char **sentence, Simple_List *simple_list_ptr);

//These functions shouldn't be visible
bool parsePipeLine(char **sentence, size_t *current_index_ptr, Simple_Command_List *simple_command_list_ptr);

bool parsePipeLine2(char **sentence, size_t *current_index_ptr, Simple_Command_List * previous_command_list);

// bool parseNewlineList(char **sentence, size_t *current_index_ptr);

bool parseSimpleCommand(char **sentence, size_t *current_index_ptr, Simple_Command_Element_List *element_list_ptr);

bool parseSimpleCommandElement(char **sentence, size_t *current_index_ptr, Simple_Command_Element *element_ptr);

bool parseWord(char **sentence, size_t *current_index_ptr);

bool parseWord2(char **sentence, size_t *current_index_ptr, size_t *current_letter_index_ptr);

bool parseLetter(char **sentence, size_t *current_index_ptr, size_t *current_letter_index_ptr);

bool parseRedirection(char **sentence, size_t *current_index_ptr, Redirection *redirection_ptr);

#endif
