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

void load (char *filedirectory, InterpreterArguments *arguments)
{
  resetInterpreterArguments(arguments);
  FILE *file;
  int character; // has to be a integer to read EOF as -1
  int bracketCounter = 0;
  int sizeOfProgram = 1024;
  int *position;

  *position = 0;

  if ((file = fopen (filedirectory, "r")) == NULL)
  {
    printf ("[ERR] reading the file failed\n");  //check if the file can be read
    //perror ("Error");
  }
  else
  {
    arguments->program_ = (char *) malloc(1024) 
    //read the file char by char and write it into an array
    while ((character = fgetc (file)) != EOF)
    { 
      //is 0 if #opened brackets == #closed brackets
      bracketCounter += check_code (arguments, character, position);

      //resizes the programmArray if the file is to long
      if (*position == sizeOfProgram - 1)
      {
        arguments->program_ = realloc (arguments->program_, sizeOfProgram * 2);
        sizeOfProgram = sizeOfProgram * 2;
      }

    }
    arguments->program_[*position] = '\0';  //end of string

    if (bracketCounter != 0)
      pritntf ("[ERR] parsing of input failed\n");
  }

  fclose (file);
}


/**********************
RUN() WILL BE DELETED
**********************/
int run (char *program, char **data, size_t *data_length,
         char **program_counter, char **data_pointer, int *breakpoints)
{

  return interpreter (program, data, data_length, program_counter,
                      data_pointer, steps, breakpoints);
}


void eval (char **data, size_t *data_length, char **data_pointer,
           char *input_bfstring)
{
  int bracketCounter = 0;
  int *position;
  int counter = 0;
  size_t len = strlen (input_bfstring);
  
  // specification: maxlength of eval input: 80 chars
  if (len > 80)
  {
    len = 80;
  }

  char new_bfcode[len];
  char *new_program_counter = new_bfcode; //pointer which points at the start
                                          // of the new_bfcode array
  
  int loopCounter;  //just for the for-loop
  for (loopCounter = 0; loopCounter < len; loopCounter++)
  {
    bracketCounter += 
    check_code (new_bfcode, input_bfstring[loopCounter], position);
  }
  new_bfcode[counter] = '\0'; //end of string

  //if #opened brackets is not #closed brackets
  if (bracketCounter != 0)
    pritntf ("[ERR] parsing of input failed\n");
  else
    interpreter (new_bfcode, data, data_length, &new_program_counter,
                 data_pointer, 0, -1);
}


int check_code (InterpreterArguments *arguments, int character,int *position)
{
  if (character == '<' || character == '>' || character == '+' ||
      character == '-' || character == '.' || character == ',' ||
      character == '[' || character == ']')
  {
    arguments->program_[position] = character;
    position++;
  } 

  if (character == '[')
    return 1;
  else if (character == ']')
    return -1;
  else
    return 0;
}