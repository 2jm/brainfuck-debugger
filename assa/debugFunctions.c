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

void load (char *filedirectory, char *program)
{
  FILE *file;
  int character; // has to be a character to read EOF as -1
  int loopCounter = 0;

  if ((file = fopen (filedirectory, "r")) == NULL)
  {
    printf ("Err at reading file\n");  //check if the file can be read
    //perror ("Error");
  }
  else
  { //read the file char by char and write it into an array
    while ((character = fgetc (file)) != EOF)
    {
      if (character == '<' || character == '>' || character == '+' ||
          character == '-' || character == '.' || character == ',' ||
          character == '[' || character == ']')
      {
        program[loopCounter] = character;
        loopCounter++;
      }
    }
    program[loopCounter] = '\0';
  }
  fclose (file);
}

int run (char *program, char **data, size_t *data_length,
         char **program_counter, char **data_pointer, int *breakpoints)
{
  int steps;
  int actualPosition;

  actualPosition = *program_counter - program;  //difference between start of
  //the array and the actual position in the bfcode

  if (breakpoints != NULL)
  {
    steps = breakpoints[0] - actualPosition;
  }
  else
  {
    steps = 0;
  }  //if the whole bfcode should be run through

  return interpreter (program, data, data_length, program_counter,
                      data_pointer, steps);
}

void eval (char **data, size_t *data_length, char **data_pointer,
           char *input_bfstring)
{
  //TODO: load befstring into code-variable (from main) when strlen(code) == 0
  //TODO: (no program currently in memory)

  int counter = 0;

  size_t len = strlen (input_bfstring);
  // spezification: maxlength of eval input: 80 chars
  if (len > 80)
  {
    len = 80;
  }

  char new_bfcode[len];
  char *new_program_counter = new_bfcode; //pointer which points at the start
  // of the new_bfcode array
  int loopCounter;
  for (loopCounter = 0; loopCounter < len; loopCounter++)
  {
    if (input_bfstring[loopCounter] == '<'
        || input_bfstring[loopCounter] == '>'
        || input_bfstring[loopCounter] == '+'
        || input_bfstring[loopCounter] == '-'
        || input_bfstring[loopCounter] == '.'
        || input_bfstring[loopCounter] == ','
        || input_bfstring[loopCounter] == '['
        || input_bfstring[loopCounter] == ']')
    {
      new_bfcode[counter] = input_bfstring[loopCounter];
      counter++;
    }
  }
  new_bfcode[counter] = '\0';

  interpreter (new_bfcode, data, data_length, &new_program_counter,
               data_pointer, 0);
}

