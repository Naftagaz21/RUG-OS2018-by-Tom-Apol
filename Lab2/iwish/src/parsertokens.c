#include "parsertokens.h"
#include <stdlib.h>
#include <stdio.h>

Simple_List * makeNewSimpleList()
{
  Simple_List *simple_list = malloc(sizeof(Simple_List));
  if(simple_list == NULL)
  {
    perror("malloc");
    return NULL;
  }

  (simple_list->pipeline).simple_command_list = makeNewSimpleCommandList(); //test whether this fails
  simple_list->hasDaemonAmpersand = 0; //default to false.
  return simple_list;
}


void freeSimpleList(Simple_List *list)
{
  if(list == NULL)
    return;
  freeSimpleCommandList((list->pipeline).simple_command_list);
  free(list);
}


Simple_Command_List * makeNewSimpleCommandList()
{
  Simple_Command_List *simple_command_list = malloc(sizeof(Simple_Command_List));
  if(simple_command_list == NULL)
  {
    perror("malloc");
    return NULL;
  }
  (simple_command_list->simple_command).element_list = makeNewSimpleCommandElementList(); //test whether this succeeds
  simple_command_list->next = NULL;
  return simple_command_list;
}


void freeSimpleCommandList(Simple_Command_List *list)
{
  if(list == NULL)
    return;
  freeSimpleCommandList(list->next);
  freeSimpleCommandElementList((list->simple_command).element_list);
  free(list);
}


Simple_Command_Element_List * makeNewSimpleCommandElementList()
{
  Simple_Command_Element_List *simple_command_element_list = malloc(sizeof(Simple_Command_Element_List));
  if(simple_command_element_list == NULL)
  {
    perror("malloc");
    return NULL;
  }
  (simple_command_element_list->element).isRedirection = -1;
  (simple_command_element_list->element).redirection_pointer = NULL;
  (simple_command_element_list->element).word = NULL;
  simple_command_element_list->next = NULL;
  return simple_command_element_list;
}


void freeSimpleCommandElementList(Simple_Command_Element_List *list)
{
  if(list == NULL)
    return;
  freeSimpleCommandElementList(list->next);
  freeRedirection((list->element).redirection_pointer);
  free(list);
}


Redirection * makeNewRedirection()
{
  Redirection *redirection = malloc(sizeof(Redirection));
  if(redirection == NULL)
  {
    perror("malloc");
    return NULL;
  }
  redirection->word = NULL;
  return redirection;
}


void freeRedirection(Redirection *redirection)
{
  if(redirection == NULL)
    return;
  free(redirection);
}
