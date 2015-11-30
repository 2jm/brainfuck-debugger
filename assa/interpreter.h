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
    char *program_;
    unsigned char *data_segment_;
    size_t data_length_;
    char *program_counter_;
    unsigned char *data_pointer_;
    int steps_;
    int *breakpoints_;
    size_t breakpoint_count_;
} InterpreterArguments;


//------------------------------------------------------------------------------
///
/// Interprets and runs the program code in the program array
///
/// @param program array where the program is stored
/// @param data double pointer to the data segment
/// @param data_length pointer to the length of the data segment
/// @param program_counter double pointer pointing to the current command
/// @param data_pointer double pointer pointing to the current data byte
/// @param steps specify the maximal count of commands to run, 0 if infinity
///
/// @return 0 if the program ran to the end
///         -1 if the program ran the specified steps
//
int interpreter(InterpreterArguments *interpreter_arguments);

#endif //ASSA_INTERPRETER_H
