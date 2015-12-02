//
//  created by matthias 24.11.2015
//

//-----------------------------------------------------------------------------
// debugFunctions.c
//
// includes all the functions needed for debugging
//
// Group: Group 13020, study assistant Angela Promitzer
//
// Authors: Jonas Juffinger 1531253
//          Matthias Klotz  1530653
//          Johannes Kopf   <Matriculum Number>
//-----------------------------------------------------------------------------
//

#include "debugFunctions.h"
#include "interpreter.h"

int load (char *filedirectory, InterpreterArguments *arguments)
{
  resetInterpreterArguments(arguments);
  FILE *file;
  int character; // has to be a integer to read EOF as -1
  int bracketCounter = 0;
  int sizeOfProgram = 1024;
  int *position = malloc(sizeof(int *));
  *position = 0;

  arguments->program_ = malloc(sizeOfProgram);

  if ((file = fopen (filedirectory, "r")) == NULL)
  {
    printf ("[ERR] reading the file failed\n");  //check if the file can be read
    return NOT_LOADED;
  }
  else
  {
    //read the file char by char and write it into an array
    while ((character = fgetc (file)) != EOF)
    {
      //is 0 if #opened brackets == #closed brackets
      bracketCounter += check_code (arguments, character, &position);

      //resizes the programmArray if the file is to long
      if (position == sizeOfProgram - 1)
      {
        sizeOfProgram *= 2;
        arguments->program_ = realloc (arguments->program_, sizeOfProgram);
      }
    }
    arguments->program_[*position] = '\0';  //end of string
    arguments->program_length_ = sizeOfProgram * sizeof(char);
    //set program_counter_ to the beginning of the code
    arguments->program_counter_ = arguments->program_;

    if (bracketCounter != 0)
      printf ("[ERR] parsing of input failed\n");
  }

  fclose (file);

  return LOADED_FROM_FILE;
}


/**********************
RUN() WILL BE DELETED
**********************/
int run (InterpreterArguments *interpreter_arguments)
{

  return interpreter (interpreter_arguments);
}


void eval (InterpreterArguments *arguments, char *input_bfstring)
{
  int bracketCounter = 0;
  int *position = malloc(sizeof(int *));
  *position = 0;
  size_t len = strlen (input_bfstring);

  // specification: maxlength of eval input: 80 chars
  if (len > 80)
  {
    len = 80;
  }

  arguments->program_ = malloc(len);
  arguments->program_length_ = len * sizeof(char);
  //set program_counter_ to the beginning of the code
  arguments->program_counter_ = arguments->program_;

  int loopCounter;  //just for the for-loop
  for (loopCounter = 0; loopCounter < len; loopCounter++)
  {
    bracketCounter +=
      check_code (arguments, input_bfstring[loopCounter], position);
  }
  arguments->program_[*position] = '\0';  //end of string

  //if #opened brackets is not #closed brackets
  if (bracketCounter != 0)
    printf ("[ERR] parsing of input failed\n");
  else
    interpreter (arguments);
}


int check_code (InterpreterArguments *arguments, int character,int *position)
{
  if (character == '<' || character == '>' || character == '+' ||
      character == '-' || character == '.' || character == ',' ||
      character == '[' || character == ']')
  {
    arguments->program_[*position] = (char)character;
    (*position)++;
  }

  if (character == '[')
    return 1;
  else if (character == ']')
    return -1;
  else
    return 0;
}