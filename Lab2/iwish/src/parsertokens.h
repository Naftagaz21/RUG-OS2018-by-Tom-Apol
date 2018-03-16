#ifndef PARSERTOKENS_H
#define PARSERTOKENS_H

typedef struct Redirection {
  int isInputRedirection; //if 1, is input redirection. if 0, is output redirection.
  char* word;
} Redirection;


typedef struct Simple_Command_Element {
  int isRedirection;
  Redirection *redirection_pointer; //if it is not a redirection, this value should be ignored.
  char* word; //if it is a redirection, this value should be ignored.
} Simple_Command_Element;


typedef struct Simple_Command_Element_List {
  Simple_Command_Element element;
  struct Simple_Command_Element_List *next;
} Simple_Command_Element_List;


typedef struct Simple_Command {
  Simple_Command_Element_List *element_list;
} Simple_Command;


typedef struct Simple_Command_List {
  Simple_Command simple_command;
  struct Simple_Command_List *next;
} Simple_Command_List;


typedef struct Pipeline {
  Simple_Command_List *simple_command_list;
} Pipeline;


typedef struct Simple_List {
  Pipeline pipeline;
  int hasDaemonAmpersand;
} Simple_List;


Simple_List * makeNewSimpleList();

void freeSimpleList(Simple_List *list);

Simple_Command_List * makeNewSimpleCommandList();

void freeSimpleCommandList(Simple_Command_List *list);

Simple_Command_Element_List * makeNewSimpleCommandElementList();

void freeSimpleCommandElementList(Simple_Command_Element_List *list);

Redirection * makeNewRedirection();

void freeRedirection(Redirection *redirection);

#endif
