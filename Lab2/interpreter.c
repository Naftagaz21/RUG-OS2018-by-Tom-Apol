#include <stdio.h>

void handleRedirection(Redirection *redirection)
{
  //TODO: implement this function
}

void handleCommandlineArgument(char *arg, DynamicArray *dynArr)
{
  DYNARR_addElem(dynArr, arg);
}

void handleCommandElementList(Simple_Command_Element_List *list, DynamicArray *dynArr)
{
  if(list == NULL)
    return;

  //TODO: do stuff
  if((list->element).isRedirection)
  {
    handleRedirection((list->element).redirection_pointer);
  }
  else
  {
    handleCommandlineArgument((list->element).word, dynArr);
  }



  handleCommandElementList(list->next, dynArr);
}

//returns 0 if successful, returns -1 otherwise.
int interpretSimpleList(Simple_List *list)
{
  if(list == NULL)
  {
    fprintf(stderr, "error in interpretSimpleList: cannot interpret NULL\n");
    return -1; //not sure if we should return 0 here
  }
  Simple_Command_List *current_command_list = (list->pipeline).simple_command_list;
  Simple_Command_Element_List * current_element_list = (current_command_list->simple_command).element_list;

  //check whether the first simple_command_element is a word.
  if(!((current_element_list->element).isRedirection))
  {
    //debug statement
    fprintf(stderr, "commands must start with a program path!\n");
    return -1;
  }

  //search for commandline args and handle i/o redirections.
  //recursively handle the current_element_list:
  //  -handle redirections
  //  -add commandline arguments to some kind of array.
  handleCommandElementList(current_element_list);




  return 0;
}
