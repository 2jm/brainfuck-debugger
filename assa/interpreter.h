//
// Created by jonas on 24.11.15.
//

#ifndef ASSA_INTERPRETER_H
#define ASSA_INTERPRETER_H

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define REGULAR_STOP 0
#define STEP_STOP 1
#define BREAKPOINT_STOP 2


typedef struct {
  int step_;
  int distance_;
} Jump;

typedef struct {
  int count_;
  size_t allocated_memory_;
  Jump *array_;
} Jumps;

typedef struct {
  char *program_; //array where the program is stored
  unsigned char **data_segment_; //pointer to the data segment
  size_t *data_length_;  //length of the data segment
  char *program_counter_; //pointer pointing to the current command
  unsigned char **data_pointer_; //pointer pointing to the current data byte
  int steps_; //the maximal count of commands to run, 0 if infinity
  int *breakpoints_;  //array with the breakpoints
  size_t breakpoint_count_; //size of the breakpoint array
  unsigned int step_counter_; //counts the steps the program makes altogether
  Jumps jumps_;
} InterpreterArguments;



//------------------------------------------------------------------------------
///
/// Interprets and runs the program code in the program array
///
/// @param interpreter_arguments pointer to an InterpreterArguments struct with all the arguments
///
/// @return  0 if the program ran to the end
///         -1 if the program ran the specified steps
///         -2 if the program stopped because of a breakpoint
//
int interpreter(InterpreterArguments *interpreter_arguments);

//------------------------------------------------------------------------------
///
/// This function returns an InterpreterArguments struct
///
/// The data_segment is 1024 bytes big an allocated
//
InterpreterArguments getUsableInterpreterArgumentsStruct(
  unsigned char **data_segment, size_t *data_length,
  unsigned char **data_pointer);

//------------------------------------------------------------------------------
///
/// Please call this function if you load a new program
///
/// @param interpreter_arguments pointer to the InterpreterArguments struct
//
void resetInterpreterArguments(InterpreterArguments *interpreter_arguments);

#endif //ASSA_INTERPRETER_H
