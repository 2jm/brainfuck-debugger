//-----------------------------------------------------------------------------
// assa.c
//
// Strings and memory management
//
// Group: 13020 study assistant Angela Promitzer
//
// Authors: Jonas Juffinger 1531253
//          Matthias Klotz  1530653
//          Johannes Kopf   1431505
//
// Latest Changes: 03.12.2015 (by Johannes Kopf)
//-----------------------------------------------------------------------------
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debugFunctions.h"
#include "interpreter.h"

#define NOT_LOADED       0
#define LOADED_FROM_FILE 1
#define LOADED_FROM_EVAL 2

void breakProgram(int program_loaded, InterpreterArguments *arguments,
                  size_t *breakpoint_size);

void step(int program_loaded, InterpreterArguments *arguments);

void memory(int program_loaded, unsigned char *data_segment,
            unsigned char *data_pointer_);

void show(int program_loaded, char *program_counter);

void change(int program_loaded, unsigned char *data_segment);

void binary(char number, char *binary_number, int digits);

int cmpfunc (const void *a, const void *b);

//------------------------------------------------------------------------------
///
/// The main program.
/// Uses to given input to print it in reversed order to the console.
///
/// @param argc not used
/// @param argv not used
///
/// @return always zero
//
int main(int argc, char *argv[])
{
  //create the interpreter argument struct
  InterpreterArguments arguments =
    getUsableInterpreterArgumentsStruct(NULL, NULL, NULL);

  int program_loaded = NOT_LOADED;
  size_t breakpoint_size = 10;
  // allocate char array for console input
  size_t line_size = 100;
  char *line = calloc(line_size, sizeof(char));

  if (argc > 2)
  {
    int argument;
    for (argument = 1; argument < argc - 1; argument++)
    {
      if (strcmp(argv[argument], "-e") == 0)
      {
        char *path = argv[argument + 1];

        load(path, &arguments);
        run(&arguments);

        freePointer((void**) &line);
        freeInterpreterArguments(&arguments);

        return 0;
      }
    }
  }

  InterpreterArguments evalArguments = getUsableInterpreterArgumentsStruct(
    arguments.data_segment_, arguments.data_length_, arguments.data_pointer_
  );


  // print first command line line output
  printf("esp> ");
  while (fgets(line, (int) line_size, stdin) != 0 && strcmp(line, "quit\n"))
  {
    // fgets adds at the end '\n\0'. Therefore override '\n' with '\0'
    line[strlen(line) - 1] = '\0';

    char *cmd;
    cmd = strtok(line, "  ");
    if (strcmp(cmd, "load") == 0)
    {
      cmd = strtok(NULL, " ");
      program_loaded = load(cmd, &arguments);
    }
    else if (strcmp(cmd, "run") == 0)
    {
      if (program_loaded != LOADED_FROM_FILE)
      {
        printf("[ERR] no program loaded\n");
        continue;
      }
      if (run(&arguments) == REGULAR_STOP) // ran to the end
      {
        // code reset
        memset(arguments.program_, 0, arguments.program_length_);

        // TODO: what should happen, if run is called twice?
        // breakpoint reset
        //memset(arguments.breakpoints_, 0, breakpoint_size * sizeof(int));
        free(arguments.breakpoints_);
        arguments.breakpoints_ = NULL;
        breakpoint_size = 10;
      }
      else // stopped at breakpoint
      {
        // shift all elements from breakpoints array one position left
        // this removes the first element/breakpoint

        //TODO do this with memmov();
        int breakp;
        for (breakp = 0; breakp < arguments.breakpoint_count_ - 1; breakp++)
        {
          arguments.breakpoints_[breakp] = arguments.breakpoints_[breakp + 1];
        }
        arguments.breakpoint_count_--;
      }
    }
    else if (strcmp(cmd, "eval") == 0)
    {
      char *bfstring = strtok(NULL, " ");
      eval(&evalArguments, bfstring);

      //only set the program_loaded variable if no program was loaded yet
      if(program_loaded == NOT_LOADED)
        program_loaded = LOADED_FROM_EVAL;
    }
    else if (strcmp(cmd, "break") == 0)
    {
      breakProgram(program_loaded, &arguments, &breakpoint_size);
    }
    else if (strcmp(cmd, "step") == 0)
    {
      step(program_loaded, &arguments);
    }
    else if (strcmp(cmd, "memory") == 0)
    {
      memory(program_loaded, *arguments.data_segment_,
             *arguments.data_pointer_);
    }
    else if (strcmp(cmd, "show") == 0)
    {
      show(program_loaded, arguments.program_counter_);
    }
    else if (strcmp(cmd, "change") == 0)
    {
      change(program_loaded, *arguments.data_segment_);
    }

    // print command line line output
    printf("esp> ");
  }

  freePointer((void**) &line);

  freeInterpreterArguments(&arguments);
  freeInterpreterArguments(&evalArguments);

  //TODO: free all variables of arguments here

  printf("Bye.\n");
  return 0;
}

void breakProgram(int program_loaded, InterpreterArguments *arguments,
                  size_t *breakpoint_size)
{
  if (program_loaded != LOADED_FROM_FILE)
  {
    printf("[ERR] no program loaded\n");
    return;
  }
  // get breakpoint position
  char *number_input = strtok(NULL, " ");
  int number = (int) strtol(number_input, (char **) NULL, 10);

  // allow only positive breakpoints
  if(number < 0)
  {
    return;
  }

  // if breakpoint with same number is already there, don't add it
  int breakp = 0;
  for (breakp = 0; breakp < arguments->breakpoint_count_; breakp++)
  {
    if (arguments->breakpoints_[breakp] == number)
    {
      return;
    }
  }

  // check if breakpoints has any memory allocated
  if (arguments->breakpoints_ == NULL)
  {
    arguments->breakpoints_ = calloc(*breakpoint_size, sizeof(int));
    arguments->breakpoint_count_ = 0;
  }
  // extend breakpoints memory if needed
  if (arguments->breakpoint_count_ + 1 > *breakpoint_size)
  {
    *breakpoint_size *= 2;
    arguments->breakpoints_ = realloc(arguments->breakpoints_,
                                      *breakpoint_size * sizeof(int));
  }

  // add next breakpoint
  arguments->breakpoints_[arguments->breakpoint_count_] = number;
  arguments->breakpoint_count_++;

//TODO: change every realloc to free the memory if NULL is returned:
//  int *new_mem = realloc (breakpoints, ++breakpoint_count * sizeof (int));
//  if (new_mem != NULL)
//  {
//    breakpoints = new_mem;
//  }
//  else
//  {
//    free (breakpoints);
//    printf ("[ERR] out of memory\\n");
//  }

  // sort breakpoints ascending (eg. 3 - 7 - 10)
  qsort(arguments->breakpoints_, arguments->breakpoint_count_, sizeof(int), cmpfunc);
  return;
}

int cmpfunc (const void *a, const void *b)
{
  return (*(int*)a - *(int*)b);
}

void step(int program_loaded, InterpreterArguments *arguments)
{
  if (program_loaded != LOADED_FROM_FILE)
  {
    printf("[ERR] no program loaded\n");
    return;
  }
  char *steps = strtok(NULL, " ");
  arguments->steps_ = (int) strtol(steps, (char **) NULL, 10);

  interpreter(arguments);
}

void memory(int program_loaded, unsigned char *data_segment,
            unsigned char *data_pointer_)
{
  if (program_loaded == NOT_LOADED)
  {
    printf("[ERR] no program loaded\n");
    return;
  }
  char *number_input = strtok(NULL, " ");
  if (number_input == NULL)
  {
    printf("Hex at %d: %x\n", (int) (data_pointer_ - data_segment),
           *data_pointer_);
    return;
  }
  // 10 = base of digit
  int number = (int) strtol(number_input, (char **) NULL, 10);

  char *type = strtok(NULL, " ");
  if (strcmp(type, "int") == 0)
  {
    printf("Integer at %d: %d\n", number, *(data_segment + number));
  }
  else if (strcmp(type, "bin") == 0)
  {
    int binary_digits = 8;
    char *binary_number = malloc(binary_digits * sizeof(char));
    //calculating the binary number
    binary(*(data_segment + number), binary_number, binary_digits);
    printf("Binary at %d: %s\n", number, binary_number);
  }
  else if (strcmp(type, "char") == 0)
  {
    printf("Char at %d: %c\n", number, *(data_segment + number));
  }
  else if (strcmp(type, "hex") == 0 || type == NULL)
  {
    printf("Hex at %d: %x\n", number, *(data_segment + number));
  }
}

void show(int program_loaded, char *program_counter)
{
  if (program_loaded != LOADED_FROM_FILE)
  {
    printf("[ERR] no program loaded\n");
    return;
  }
  char *size_input = strtok(NULL, " ");
  // 10 is default size
  int size = (size_input != NULL) ? (int) strtol(size_input, (char **) NULL, 10) : 10;

  // Source: http://stackoverflow
  // .com/questions/4214314/get-a-substring-of-a-char
  // print "size" characters from target string
  // at code position + offset
  printf("%.*s\n", size, program_counter);
}

void change(int program_loaded, unsigned char *data_segment)
{
  if (program_loaded == NOT_LOADED)
  {
    printf("[ERR] no program loaded\n");
    return;
  }

  char *number_input = strtok(NULL, " ");
  char *end_ptr;
  int number = (int) strtol(number_input, &end_ptr, 10);
  if (end_ptr == number_input) // conversion failed
  {
    // default number: current position
    number = 0;
  }

  char *hex_byte_input = strtok(NULL, " ");
  int hex_byte = (int) strtol(hex_byte_input, &end_ptr, 16);
  if (end_ptr == hex_byte_input) // conversion failed
  {
    // default hex_byte: 0x0
    hex_byte = 0;
  }
  *(data_segment + number) = (unsigned char) hex_byte;
}

void binary(char number, char *binary_number, int digits)
{
  // fill binary_number array from the end, to let the binary number be correct
  int counter = digits - 1;

  while (number)
  {
    if (number % 2 == 0)
    {
      binary_number[counter] = '0';
    }
    else
    {
      binary_number[counter] = '1';
    }
    number /= 2;
    counter--;
  }

  for (; counter >= 0; counter--)
  {
    binary_number[counter] = '0';
  }

  binary_number[digits] = '\0';
}