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

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define NOT_LOADED       0
#define LOADED_FROM_FILE 1
#define LOADED_FROM_EVAL 2

#define REGULAR_STOP 0
#define STEP_STOP 1
#define BREAKPOINT_STOP 2

#define SUCCESS 1

#define FILE_PARSE_ERROR -3
#define FILE_READ_ERROR -4

#define WRONG_USAGE_RETURN_CODE 1
#define OUT_OF_MEMORY_RETURN_CODE 2
#define FILE_PARS_ERROR_RETURN_CODE 3
#define FILE_READ_ERROR_RETURN_CODE 4


typedef struct {
  int step_;
  int distance_;
} Jump;

typedef struct {
  unsigned int count_;
  size_t allocated_memory_;
  Jump *array_;
} Jumps;

typedef struct {
  char *program_; //array where the program is stored
  size_t program_length_; //size of the whole program array
  unsigned char **data_segment_; //pointer to the data segment
  size_t *data_length_;  //length of the data segment
  char *program_counter_; //pointer pointing to the current command
  unsigned char **data_pointer_; //pointer pointing to the current data byte
  int steps_; //the maximal count of commands to run, 0 if infinity
  int *breakpoints_;  //array with the breakpoints
  size_t breakpoint_count_; //size of the breakpoint array
  unsigned int step_counter_; //counts the steps the program makes altogether
  Jumps jumps_;
  char** jump_points_;
  int *size_of_jump_points_;
} InterpreterArguments;



void exitWrongUsage();

void breakProgram(int program_loaded, InterpreterArguments *arguments,
                  size_t *breakpoint_size);

int compareFunction (const void *a, const void *b);

void step(int program_loaded, InterpreterArguments *arguments, int bonus);

void memory(int program_loaded, unsigned char *data_segment,
            unsigned char *data_pointer_);

void show(int program_loaded, char *program_counter);

void change(int program_loaded, unsigned char *data_segment,
            unsigned char *data_pointer);

void binary(char number, char *binary_number, int digits);



//-----------------------------------------------------------------------------
///
/// Loading the code character by character out of a file and write it into an
/// array.
///
/// @param *filedirectory directory of the codefile to fetch
/// @param *program Array in which the bf-code gets written
///
/// @return 0 when not loaded (error), LOADED_FROM_FILE when loaded
//
int load(char *filedirectory, InterpreterArguments *interpreter_arguments,
         int bonus);

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
int run(InterpreterArguments *interpreter_arguments);

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
void eval(InterpreterArguments *arguments, char *input_bfstring, int bonus);

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
               int *position, int bonus);

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
/// Please call this function if you load a new program
///
/// @param interpreter_arguments pointer to the InterpreterArguments struct
//
void resetInterpreterArguments(InterpreterArguments *interpreter_arguments);

//------------------------------------------------------------------------------
///
/// This function returns an InterpreterArguments struct
///
/// The data_segment is 1024 bytes big an allocated
//
InterpreterArguments getUsableInterpreterArgumentsStruct(
  unsigned char **data_segment, size_t *data_length,
  unsigned char **data_pointer);

void freeInterpreterArguments(InterpreterArguments *interpreterArguments);

void freePointer(void **pointer);

void freeDoublePointer(void ***pointer);

void *saveMalloc(size_t size);

void *saveRealloc(void* pointer, size_t size);

void expandDataSegment (unsigned char **data_segment, size_t *data_length);

void processLoop(InterpreterArguments *interpreter_arguments, int direction);

int jumpToMatchingBrace(InterpreterArguments *interpreter_arguments);

void insertJump(Jumps *jumps, Jump jump);

void newJumpPoint(InterpreterArguments *interpreter_arguments);


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

  int command_line_argument_b = 0;
  int command_line_argument_e = 0;
  char *command_line_argument_path = NULL;
  int eof = 0;

  if (argc >= 2)
  {
    int argument;
    for (argument = 1; argument < argc; argument++)
    {
      if (strcmp(argv[argument], "-b") == 0)
      {
        command_line_argument_b = 1;
      }
      else if (strcmp(argv[argument], "-e") == 0)
      {
        if(argc >= argument + 2)
        {
          command_line_argument_e = 1;
          argument++;
          command_line_argument_path = argv[argument];
        }
        else
        {
          exitWrongUsage();
        }
      }
      else
      {
        exitWrongUsage();
      }
    }
  }

  if(command_line_argument_e == 1)
  {
    int return_code = load(command_line_argument_path, &arguments,
                           command_line_argument_b);

    if(return_code == FILE_READ_ERROR)
    {
      return FILE_READ_ERROR_RETURN_CODE;
    }
    if(return_code == FILE_PARSE_ERROR)
    {
      return FILE_PARS_ERROR_RETURN_CODE;
    }

    run(&arguments);

    freePointer((void**) &line);
    freeInterpreterArguments(&arguments);

    return EXIT_SUCCESS;
  }



  InterpreterArguments evalArguments = getUsableInterpreterArgumentsStruct(
    arguments.data_segment_, arguments.data_length_, arguments.data_pointer_
  );

  // print first command line line output
  printf("esp> ");
  while (fgets(line, (int) line_size, stdin) != 0 && strcmp(line, "quit\n"))
  {
    eof = 0;  //if there is an input it's not EOF
    // fgets adds at the end '\n\0'. Therefore override '\n' with '\0'
    line[strlen(line) - 1] = '\0';

    char *cmd;
    cmd = strtok(line, "  ");
    if (cmd)
    {
      if (strcmp(cmd, "load") == 0)
      {
        cmd = strtok(NULL, " ");
        if(cmd != NULL)
        {
          int return_code = load(cmd, &arguments, command_line_argument_b);
          if(return_code == SUCCESS)
          {
            program_loaded = LOADED_FROM_FILE;
          }
          else
          {
            program_loaded = NOT_LOADED;
          }
        }
        else
        {
          printf("[ERR] wrong parameter count\n");
        }
      }
      else if (strcmp(cmd, "run") == 0)
      {
        if (program_loaded != LOADED_FROM_FILE)
        {
          printf("[ERR] no program loaded\n");
        }
        else
        {
          int stop_reason = run(&arguments);

          if (stop_reason == REGULAR_STOP) // ran to the end
          {
            // code reset
            memset(arguments.program_, 0, arguments.program_length_);

            // TODO: what should happen, if run is called twice?
            // breakpoint reset
            //memset(arguments.breakpoints_, 0, breakpoint_size * sizeof(int));
            free(arguments.breakpoints_);
            arguments.breakpoints_ = NULL;
            breakpoint_size = 10;

            program_loaded = NOT_LOADED;
          }
          else if(stop_reason == STEP_STOP)
          {
            //should never be reached
          }
          else if(stop_reason == BREAKPOINT_STOP) // stopped at breakpoint
          {
            // shift unused breakpoints left to remove already used breakpoint
            memmove(arguments.breakpoints_, arguments.breakpoints_ + 1, (arguments.breakpoint_count_ - 1) * sizeof(int));
            arguments.breakpoint_count_--;
          }
        }
      }
      else if (strcmp(cmd, "eval") == 0)
      {
        char *bfstring = strtok(NULL, " ");
        eval(&evalArguments, bfstring, command_line_argument_b);

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
        step(program_loaded, &arguments, command_line_argument_b);
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
        change(program_loaded, *arguments.data_segment_,
               *arguments.data_pointer_);
      }
    }
    else // Das kann man weglassen if(!cmd)
    {
      eof = 1;  //if there's no input, it could be EOF
                //if it's not, loop will run again
    }

    // print command line line output
    printf("esp> ");
  }

  freePointer((void**) &line);

  freeInterpreterArguments(&arguments);

  // set this pointers to NULL so that they are not freed a second time
  evalArguments.data_segment_ = NULL;
  evalArguments.data_pointer_ = NULL;
  evalArguments.data_length_  = NULL;
  freeInterpreterArguments(&evalArguments);

  if (!eof)
    printf("Bye.\n");

  return EXIT_SUCCESS;
}

void exitWrongUsage()
{
  printf("[ERR] usage: ./assa [-e brainfuck_filnename]\n");
  exit(WRONG_USAGE_RETURN_CODE);
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
  if(number_input == NULL)
  {
    printf("[ERR] wrong parameter count\n");
    return;
  }

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
    arguments->breakpoints_ = saveRealloc(arguments->breakpoints_,
                                      *breakpoint_size * sizeof(int));
  }

  // add next breakpoint
  arguments->breakpoints_[arguments->breakpoint_count_] = number;
  arguments->breakpoint_count_++;

  // sort breakpoints ascending (eg. 3 - 7 - 10)
  qsort(arguments->breakpoints_, arguments->breakpoint_count_, sizeof(int), compareFunction);
  return;
}

int compareFunction (const void *a, const void *b)
{
  return (*(int*)a - *(int*)b);
}

void step(int program_loaded, InterpreterArguments *arguments, int bonus)
{
  if (program_loaded != LOADED_FROM_FILE)
  {
    printf("[ERR] no program loaded\n");
    return;
  }
  char *steps = strtok(NULL, " ");
  arguments->steps_ = (int) strtol(steps, (char **) NULL, 10);

  if(arguments->steps_ == 0 || (arguments->steps_ < 0 && bonus == 0))
  {
    // don't call the interpreter with 0 steps because it would run indefinitely
    // but just to nothing.
    arguments->steps_ = 0;  //set it 0 if it is < 0
    return;
  }

  interpreter(arguments);

  arguments->steps_ = 0;
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
  if (type == NULL || strcmp(type, "hex") == 0)
  {
    printf("Hex at %d: %x\n", number, *(data_segment + number));
  }
  else if (strcmp(type, "int") == 0)
  {
    printf("Integer at %d: %d\n", number, *(data_segment + number));
  }
  else if (strcmp(type, "bin") == 0)
  {
    int binary_digits = 8;
    char binary_number[9];
    //calculating the binary number
    binary(*(data_segment + number), binary_number, binary_digits);
    printf("Binary at %d: %s\n", number, binary_number);
  }
  else if (strcmp(type, "char") == 0)
  {
    printf("Char at %d: %c\n", number, *(data_segment + number));
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

void change(int program_loaded, unsigned char *data_segment,
            unsigned char *data_pointer)
{
  if (program_loaded == NOT_LOADED)
  {
    printf("[ERR] no program loaded\n");
    return;
  }

  char *number_input = strtok(NULL, " ");
  if(number_input == NULL)
  {
    *data_pointer = 0;
    return;
  }
  char *end_ptr;
  int number = (int) strtol(number_input, &end_ptr, 10);


  char *hex_byte_input = strtok(NULL, " ");
  if(hex_byte_input == NULL)
  {
    data_segment[number] = 0;
    return;
  }
  int hex_byte = (int) strtol(hex_byte_input, &end_ptr, 16);
  data_segment[number] = (unsigned char) hex_byte;
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

int load (char *file_directory, InterpreterArguments *arguments, int bonus)
{
  resetInterpreterArguments(arguments);
  FILE *file;
  int character; // has to be a integer to read EOF as -1
  int bracket_counter = 0;
  size_t program_size = 1024;
  int position = 0;

  arguments->program_ = saveMalloc(program_size);


  if ((file = fopen (file_directory, "r")) == NULL)
  {
    printf ("[ERR] reading the file failed\n");  //check if the file can be read
    return FILE_READ_ERROR;
  }
  else
  {
    //read the file char by char and write it into an array
    while ((character = fgetc (file)) != EOF)
    {
      //is 0 if #opened brackets == #closed brackets
      bracket_counter += check_code (arguments, character, &position, bonus);

      //resizes the programmArray if the file is too long
      if (position == program_size - 1)
      {
        program_size *= 2;
        arguments->program_ = realloc (arguments->program_, program_size);
      }
    }
    arguments->program_[position] = '\0';  //end of string
    arguments->program_length_ = program_size * sizeof(char);
    //set program_counter_ to the beginning of the code
    arguments->program_counter_ = arguments->program_;

    if (bracket_counter != 0)
    {
      printf("[ERR] parsing of input failed\n");
      return FILE_PARSE_ERROR;
    }
  }

  fclose (file);

  return LOADED_FROM_FILE;
}

/**********************
RUN() WILL BE DELETED
**********************/
int run (InterpreterArguments *interpreter_arguments)
{

  return interpreter (interpreter_arguments);
}

void eval (InterpreterArguments *arguments, char *input_bfstring, int bonus)
{
  int bracket_counter = 0;
  int position = 0;
  size_t len = strlen (input_bfstring);

  // specification: maxlength of eval input: 80 chars
  if (len > 80)
  {
    len = 80;
  }

  arguments->program_length_ = len+1;
  arguments->program_ = saveMalloc(arguments->program_length_ * sizeof(char));
  //set program_counter_ to the beginning of the code
  arguments->program_counter_ = arguments->program_;

  int loopCounter;  //just for the for-loop
  for (loopCounter = 0; loopCounter < len; loopCounter++)
  {
    bracket_counter +=
      check_code (arguments, input_bfstring[loopCounter], &position, bonus);
  }
  arguments->program_[position] = '\0';  //end of string

  //if #opened brackets is not #closed brackets
  if (bracket_counter != 0)
    printf ("[ERR] parsing of input failed\n");
  else
  {
    interpreter(arguments);
    freePointer((void**) &arguments->program_);
  }
}

int check_code (InterpreterArguments *arguments, int character,int *position,
                int bonus)
{
  if (character == '<' || character == '>' || character == '+' ||
      character == '-' || character == '.' || character == ',' ||
      character == '[' || character == ']')
  {
    arguments->program_[*position] = (char)character;
    (*position)++;
  }
  else if((bonus && character == '&') || (bonus && character == '%'))
  {
    arguments->program_[*position] = (char)character;
    (*position)++;
  }

  if (character == '[')
    return 1;
  else if (character == ']')
    return -1;
  else
    return 0;
}

int interpreter (InterpreterArguments *interpreter_arguments)
{
  // non static variables
  const int direction = (interpreter_arguments->steps_ < 0) ? -1 : 1;

  const char move_forward = (direction == 1) ? (char)'>' : (char)'<';
  const char move_back    = (direction == 1) ? (char)'<' : (char)'>';
  const char increment    = (direction == 1) ? (char)'+' : (char)'-';
  const char decrement    = (direction == 1) ? (char)'-' : (char)'+';
  // print and read do not change
  const char loop_start   = (direction == 1) ? (char)'[' : (char)']';
  const char loop_end     = (direction == 1) ? (char)']' : (char)'[';

  const char jump_point   = (char)'&';
  const char jump         = (char)'%';

  int break_loop = 0;

  //create a local variable for steps because it should not be changed
  //if -1 runs infinitely long (to the end of the program)
  int steps = (interpreter_arguments->steps_ == 0) ? -1 : interpreter_arguments->steps_;

  //the program must run shifted one to the left, increment at the end
  if(direction == -1)
    interpreter_arguments->program_counter_--;

  for (; *(interpreter_arguments->program_counter_) != 0 && steps != 0;
         steps -= direction)
  {
    //check if a breakpoint is reached
    int *breakpoint = interpreter_arguments->breakpoints_;
    for (; breakpoint < interpreter_arguments->breakpoints_ + interpreter_arguments->breakpoint_count_; breakpoint++)
    {
      if (interpreter_arguments->program_ + *breakpoint == interpreter_arguments->program_counter_)
      {
        break_loop = 1;
      }
    }
    if (break_loop)
      break;

    //printf("%lu %c: ", interpreter_arguments->program_counter_ - interpreter_arguments->program_, *(interpreter_arguments->program_counter_));

    // ------------  >  ------------
    if (*(interpreter_arguments->program_counter_) == move_forward)
    {
      if (++(*interpreter_arguments->data_pointer_) >
          *interpreter_arguments->data_segment_ + *(interpreter_arguments->data_length_))
      {
        expandDataSegment(interpreter_arguments->data_segment_, interpreter_arguments->data_length_);
      }
      interpreter_arguments->program_counter_ += direction;
    }


      // ------------  <  ------------
    else if (*(interpreter_arguments->program_counter_) == move_back)
    {
      if (--(*interpreter_arguments->data_pointer_) < *interpreter_arguments->data_segment_)
      {
        (*interpreter_arguments->data_pointer_)++;
      }

      interpreter_arguments->program_counter_ += direction;
    }


      // ------------  +  ------------
    else if (*(interpreter_arguments->program_counter_) == increment)
    {
      (**interpreter_arguments->data_pointer_)++;

      interpreter_arguments->program_counter_ += direction;
    }


      // ------------  -  ------------
    else if (*(interpreter_arguments->program_counter_) == decrement)
    {
      (**interpreter_arguments->data_pointer_)--;

      interpreter_arguments->program_counter_ += direction;
    }


      // ------------  .  ------------
    else if (*(interpreter_arguments->program_counter_) == '.')
    {
      putchar(**interpreter_arguments->data_pointer_);

      interpreter_arguments->program_counter_ += direction;
    }


      // ------------  .  ------------
    else if (*(interpreter_arguments->program_counter_) == ',')
    {
      **interpreter_arguments->data_pointer_ = (unsigned char) getchar ();

      interpreter_arguments->program_counter_ += direction;
    }


      // ------------  [  ------------
    else if (*(interpreter_arguments->program_counter_) == loop_start)
    {
      processLoop(interpreter_arguments, direction);
    }


      // ------------  ]  ------------
    else if (*(interpreter_arguments->program_counter_) == loop_end)
    {
      processLoop(interpreter_arguments, direction);
    }


      // ------------  &  ------------
    else if (*(interpreter_arguments->program_counter_) == jump_point)
    {
      newJumpPoint(interpreter_arguments);
      //**interpreter_arguments->data_pointer_ = 0;

      interpreter_arguments->program_counter_ += direction;
    }


      // ------------  %  ------------
    else if (*(interpreter_arguments->program_counter_) == jump)
    {
      interpreter_arguments->program_counter_ =
        interpreter_arguments->jump_points_[**interpreter_arguments->data_pointer_];

      interpreter_arguments->program_counter_ += direction;
    }

    interpreter_arguments->step_counter_ += direction;

    //printf(" step: %d\n", interpreter_arguments->step_counter_-1);
  }

  if(direction == -1)
    interpreter_arguments->program_counter_++;

  if (steps == 0) //program ran the specified steps to the end
  {
    return STEP_STOP;
  }

  if (break_loop)
  {
    return BREAKPOINT_STOP;
  }

  return REGULAR_STOP;
}

void resetInterpreterArguments(InterpreterArguments *interpreter_arguments)
{
  interpreter_arguments->step_counter_ = 0;
  interpreter_arguments->jumps_.count_ = 0;

  memset (*interpreter_arguments->data_segment_, 0,
          *interpreter_arguments->data_length_ * sizeof(unsigned char));

  *interpreter_arguments->data_pointer_ = *interpreter_arguments->data_segment_;

  interpreter_arguments->breakpoint_count_ = 0;
}

InterpreterArguments getUsableInterpreterArgumentsStruct(
  unsigned char **data_segment, size_t *data_length,
  unsigned char **data_pointer)
{
  InterpreterArguments interpreter_arguments = {
    NULL,
    0,
    data_segment,
    data_length,
    NULL,
    data_pointer,
    0,
    NULL,
    0,
    0,
    {0, 0, NULL},
    NULL,
    NULL
  };

  if(data_segment == NULL)
  {
    interpreter_arguments.data_length_ = saveMalloc(sizeof(size_t));
    *interpreter_arguments.data_length_ = 1024;

    interpreter_arguments.data_segment_ = saveMalloc(sizeof(unsigned char*));
    *interpreter_arguments.data_segment_ =
      calloc(*interpreter_arguments.data_length_, sizeof(unsigned char));

    interpreter_arguments.data_pointer_ = saveMalloc(sizeof(unsigned char*));
    *interpreter_arguments.data_pointer_ = *interpreter_arguments.data_segment_;
  }

  interpreter_arguments.jumps_.allocated_memory_ = 200;
  interpreter_arguments.jumps_.array_ = (Jump*) saveMalloc(
    interpreter_arguments.jumps_.allocated_memory_ * sizeof(Jump));

  //*interpreter_arguments.jump_points_ = saveMalloc(1);

  return interpreter_arguments;
}

void freeInterpreterArguments(InterpreterArguments *interpreterArguments)
{
  freePointer((void**) &(interpreterArguments->program_));
  freeDoublePointer((void***) &(interpreterArguments->data_segment_));
  freePointer((void**) &(interpreterArguments->data_length_));
  freePointer((void**) &(interpreterArguments->data_pointer_));
  freePointer((void**) &(interpreterArguments->breakpoints_));
  freePointer((void**) &(interpreterArguments->jumps_.array_));
}

void freePointer(void **pointer)
{
  if(*pointer != NULL)
  {
    free(*pointer);
    *pointer = NULL;
  }
}

void freeDoublePointer(void ***pointer)
{
  if(*pointer != NULL)
  {
    freePointer(*pointer);

    free(*pointer);
    *pointer = NULL;
  }
}

void *saveMalloc(size_t size)
{
  void *pointer = malloc(size);
  if(pointer == NULL)
  {
    printf("[ERR] out of memory\n");
    exit(OUT_OF_MEMORY_RETURN_CODE);
  }
  return pointer;
}

void *saveRealloc(void* pointer, size_t size)
{
  pointer = realloc(pointer, size);
  if(pointer == NULL)
  {
    printf("[ERR] out of memory\n");
    exit(OUT_OF_MEMORY_RETURN_CODE);
  }
  return pointer;
}

void expandDataSegment (unsigned char **data_segment, size_t *data_length)
{
  *data_length *= 2;
  *data_segment = realloc (*data_segment, *data_length);

  //set the new memory to 0
  memset (*data_segment + (*data_length) / 2, 0, (*data_length) / 2);
}

void processLoop(InterpreterArguments *interpreter_arguments, int direction)
{
  if (direction == 1)
  {
    if (
      (
        *(interpreter_arguments->program_counter_) == '[' &&
        **interpreter_arguments->data_pointer_ == 0
      )
      ||
      (
        *(interpreter_arguments->program_counter_) == ']' &&
        **interpreter_arguments->data_pointer_ != 0
      )
      )
    {
      int distance = jumpToMatchingBrace(interpreter_arguments);
      insertJump(&interpreter_arguments->jumps_,
                 (Jump){interpreter_arguments->step_counter_, distance});
    }
    else
    {
      interpreter_arguments->program_counter_++;
    }
  }
  else if (direction == -1)
  {
    //search in the jumps array if the program was jumping
    int search_loop_index;
    int jump_found = 0;

    //search from back because, the chance that the jump is at the end is high
    for (search_loop_index = interpreter_arguments->jumps_.count_;
         search_loop_index > 0; search_loop_index++)
    {
      //if the program was jumping
      if (interpreter_arguments->jumps_.array_[search_loop_index].step_ ==
          interpreter_arguments->step_counter_ - 1)
      {
        //jump in the opposite direction
        interpreter_arguments->program_counter_ -=
          interpreter_arguments->jumps_.array_[search_loop_index].distance_;

        jump_found = 1;

        //printf("Program was jumping on step %d, jumping back by %d", jumps->array_[search_loop_index].step_, -jumps->array_[search_loop_index].distance_);
      }
    }

    if (jump_found == 0)
      interpreter_arguments->program_counter_--;
  }
}

int jumpToMatchingBrace(InterpreterArguments *interpreter_arguments)
{
  int loop_counter = 1;
  int distance = 0;
  int direction = (*(interpreter_arguments->program_counter_) == '[') ? 1 : -1;

  distance += direction;
  interpreter_arguments->program_counter_ += direction;

  while (loop_counter > 0)
  {
    if (*(interpreter_arguments->program_counter_) == '[')
    {
      loop_counter += direction;
    }
    else if (*(interpreter_arguments->program_counter_) == ']')
    {
      loop_counter -= direction;
    }

    distance += direction;
    interpreter_arguments->program_counter_ += direction;
  }

  if(direction == -1)
  {
    interpreter_arguments->program_counter_ += 2;
    distance += 2;
  }

  return distance;
}

void insertJump(Jumps *jumps, Jump jump)
{
  jumps->array_[jumps->count_] = jump;

  jumps->count_++;

  if(jumps->count_ == jumps->allocated_memory_)
  {
    //extend array
    jumps->allocated_memory_ *= 2;
    jumps->array_ = saveRealloc(jumps->array_, jumps->allocated_memory_ * sizeof(Jump));
  }
}

void newJumpPoint(InterpreterArguments *interpreter_arguments)
{
  if (**interpreter_arguments->data_pointer_ >
      *interpreter_arguments->size_of_jump_points_)
  {
    interpreter_arguments->jump_points_ =
      saveRealloc(interpreter_arguments->jump_points_ , sizeof(char) *
                                                    **interpreter_arguments->data_pointer_);

    *interpreter_arguments->size_of_jump_points_ =
      **interpreter_arguments->data_pointer_;
  }

  interpreter_arguments->jump_points_[**interpreter_arguments->data_pointer_] =
    interpreter_arguments->program_counter_;


}