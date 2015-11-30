//
// Created by jonas on 24.11.15.
//

#ifndef ASSA_INTERPRETER_H
#define ASSA_INTERPRETER_H

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct {
    char *program_; //array where the program is stored
    unsigned char *data_segment_; //pointer to the data segment
    size_t data_length_;  //length of the data segment
    char *program_counter_; //pointer pointing to the current command
    unsigned char *data_pointer_; //pointer pointing to the current data byte
    int steps_; //the maximal count of commands to run, 0 if infinity
    int *breakpoints_;  //array with the breakpoints
    size_t breakpoint_count_; //size of the breakpoint array
} InterpreterArguments;


//------------------------------------------------------------------------------
///
/// Interprets and runs the program code in the program array
///
/// @param interpreter_arguments pointer to an InterpreterArguments struct with all the arguments
///
/// @return 0 if the program ran to the end
///         -1 if the program ran the specified steps
///         -2 if the program stopped because of a breakpoint
//
int interpreter(InterpreterArguments *interpreter_arguments);

#endif //ASSA_INTERPRETER_H
