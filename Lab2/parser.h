#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stddef.h>

bool parseInput(char **sentence);

//These functions shouldn't be visible
bool parsePipeLine(char **sentence, size_t *current_index_ptr);

bool parsePipeLine2(char **sentence, size_t *current_index_ptr);

bool parseNewlineList(char **sentence, size_t *current_index_ptr);

bool parseSimpleCommand(char **sentence, size_t *current_index_ptr);

bool parseSimpleCommandElement(char **sentence, size_t *current_index_ptr);

bool parseWord(char **sentence, size_t *current_index_ptr);

bool parseWord2(char **sentence, size_t *current_index_ptr, size_t *current_letter_index_ptr);

bool parseLetter(char **sentence, size_t *current_index_ptr, size_t *current_letter_index_ptr);

bool parseRedirection(char **sentence, size_t *current_index_ptr);

#endif
