//
// Created by matthias on 24.11.15.
//

//-----------------------------------------------------------------------------
// debugFunctions.h
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

#ifndef ASSA_DEBUGFUNCTIONS_H
#define ASSA_DEBUGFUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include "interpreter.h"

//-----------------------------------------------------------------------------
///
/// Loading the code character by character out of a file and write it into an 
/// array.
///
/// @param *filedirectory directory of the codefile to fetch
/// @param *program Array in which the bf-code gets written
///
//
void load(char *filedirectory, InterpreterArguments *interpreter_arguments);

//-----------------------------------------------------------------------------
///
/// Executing the bf-code for a set number of steps (or till the code ends) by 
/// calling the interpreter
///
/// @param *program Array in which the bf-code is saved
/// @param **data   Datasegment
/// @param *data_length length of the datasegment
/// @param **programm_counter counter for position in the code where the
///        interpreter should start
/// @param **data_pointer points at the actual position in the datasegment
/// @param *breakpoints array where the positions of the breakpoints are saved
///
//
int run(char *program, char **data, size_t *data_length,char **program_counter,
        char **data_pointer, int *breakpoints);

//-----------------------------------------------------------------------------
///
/// Manipulating the datasegment with a user entered bf-code
///
/// @param **data   Datasegment
/// @param *data_length length of the datasegment
/// @param *program Array in which the bf-code gets written
/// @param **data_pointer points at the actual position in the datasegment
/// @param *input_code the bf-code to execute, which was given with the
///         funtionccall
///
//
void eval(InterpreterArguments *interpreter_arguments, char *input_bfstring);

//-----------------------------------------------------------------------------
///
/// Checking single character if it is bfCode and checking if bracket is
/// opened or closed
///
/// @param *programArray  array in which the correct code is written
/// @param character the sign which should be checked
/// @param *position the position where in the array the char will be written
///
/// @return 1   if a bracket is opened
/// @return -1  if a bracket is closed
/// @return 0   if no bracket is opened or closed
///
//
int check_code(InterpreterArguments *interpreter_arguments, int character,
              int *position);

#endif //ASSA_DEBUGFUNCTIONS_H
