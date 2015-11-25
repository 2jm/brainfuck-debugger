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


void load(char *filedirectory, char *program)
{
  FILE *file;
  char character;
  int loopCounter;

  if((file = fopen(filedirectory, "r")) == NULL)
    printf("Err at reading file");
  else
  {
    while((character = fgetc(file)) != EOF)
    {
      program[loopCounter] = character;
      loopCounter++;
    }
  }
}


void run(char *program, char **data, size_t *data_length,char **program_counter,
        char **data_pointer, int *breakpoints)
{
  int steps;
  int actualPosition;

  actualPosition = *program_counter - program;

  steps = breakpoints[0] - actualPosition;

  int interpreter(char *program, char **data, size_t *data_length,
                 char **program_counter, char **data_pointer, steps);

}


void eval(char **data, size_t *data_length, char **data_pointer, 
          char *input_code)
{
  
}
