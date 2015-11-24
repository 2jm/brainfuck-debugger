//
// Created by jonas on 24.11.15.
//

#ifndef ASSA_INTERPRETER_H
#define ASSA_INTERPRETER_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

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

int interpreter(char *program, char **data_segment, size_t *data_length,
                  char **program_counter, char **data_pointer, int steps);

#endif //ASSA_INTERPRETER_H
