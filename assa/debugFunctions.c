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
  int character;
  int loopCounter;


  if((file = fopen(filedirectory, "r")) == NULL)
    printf("Err at reading file");  //check if the file can be read
  else
  { //read the file char by char and write it into an array
    while((character = fgetc(file)) != EOF)
    {
      if(character == '<' || '>' || '+' || '-' || '.' || ',' || '[' || ']')
      {
      program[loopCounter] = character;
      loopCounter++;        
      }
    }
  }
}


void run(char *program, char **data, size_t *data_length,char **program_counter,
        char **data_pointer, int *breakpoints)
{
  int steps;
  int actualPosition;

  actualPosition = *program_counter - program;  //difference between start of
                                                //the array and the actual 
                                                //position in the bfcode
  if(breakpoints[0] != 0)
    steps = breakpoints[0] - actualPosition;
  else
    steps = 0;  //if the whole bfcode should be run through

  interpreter(program, data, data_length, program_counter, 
                  data_pointer, steps);

}


void eval(char **data, size_t *data_length, char **data_pointer, 
          char *input_bfstring)
{
  int len;

  int counter;
  int loopCounter;

  len = strlen(input_bfstring);

  char new_bfcode[len];
  char *new_program_counter;

  new_program_counter = new_bfcode; //pointer which points at the start of the
                                    //new_bfcode array
  for(loopCounter = 0; loopCounter<len; loopCounter++)
  {
    if(input_bfstring[loopCounter] == '<' || '>' || '+' ||'-' || '.' || ',' || 
      '[' || ']')
    {
      new_bfcode[counter] = input_bfstring[loopCounter];
      counter++;        
    }
  }

  interpreter(new_bfcode, data, data_length, &new_program_counter, 
                  data_pointer, 0);
}

