//------------------------------------------------------------------------------
// assa.c
//
// Assignment A - Brainfuck “gdb”
//
// Group: 13020 study assistant Angela Promitzer
//
// Authors: Jonas Juffinger 1531253
//          Matthias Klotz  1530653
//          Johannes Kopf   1431505
//
// Latest Changes: 08.12.2015 (by Jonas Juffinger)
//------------------------------------------------------------------------------
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


//#define __BENCHMARK__
#ifdef __BENCHMARK__
#include <time.h>
#endif


#define NOT_LOADED       0
#define LOADED_FROM_EVAL 1
#define LOADED_FROM_FILE 2
#define RUN_FINISHED     3

#define DATA_SEGMENT_AVAILABLE 1

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


//------------------------------------------------------------------------------
///
/// This struct holds the breakpoints in an array
//
typedef struct
{
  int *array_;                //array with the breakpoints
  size_t allocated_memory_;   //size of the breakpoint array
  size_t count_;              //number of active breakpoints
} Breakpoints;


//------------------------------------------------------------------------------
///
/// This struct holds a single jump in the program, it's needed for reverse step
///
/// e.g. +++>[+.-]+++ This program jumps on step 5 the distance 4
//
typedef struct
{
  unsigned long long step_;      //defines at what step the program was jumping
  int distance_;  //the jumps distance, positive or negative
} MadeJump;


//------------------------------------------------------------------------------
///
/// This struct holds all the jumps in an array
//
typedef struct
{
  unsigned int count_;           //count of the jumps
  size_t allocated_memory_;      //size of the MadeJump array
  MadeJump *array_;              //array where the jumps are stored
} MadeJumps;


//------------------------------------------------------------------------------
///
/// This struct holds a single jumppoint for the jump extension
//
typedef struct
{
  char *address_;
  int id_;
} JumpPoint;


//------------------------------------------------------------------------------
///
/// This struct holds all values overwritten by a user input
//
typedef struct
{
  unsigned long long step_;
  unsigned char value_;
} OverwrittenDataByte;


//-----------------------------------------------------------------------------
///
/// This struct holds all the overwritten values in an array
//
typedef struct
{
  unsigned int count_;           //count of the jumps
  size_t allocated_memory_;      //size of the MadeJump array
  OverwrittenDataByte *array_;   //array where the jumps are stored
} OverwrittenDataBytes;


//------------------------------------------------------------------------------
///
/// This struct holds all the variables the interpreter needs to run
//
typedef struct
{
  char *program_;                //array where the program is stored
  size_t program_length_;        //size of the program array
  int *jump_cache_;              //array holding the calculated jumps for speed
  unsigned char **data_segment_; //pointer to the data segment
  size_t *data_length_;          //length of the data segment
  char *program_counter_;        //pointer pointing to the current command
  unsigned char **data_pointer_; //pointer pointing to the current data byte
  long long steps_;              //the maximal steps to run, 0 if infinity
  Breakpoints breakpoints_;
  unsigned long long step_counter_;   //counts the steps the program makes
  int activate_reverse_step_;
  MadeJumps made_jumps_;                  //instance of the MadeJumps struct
  OverwrittenDataBytes overwrittenDataBytes_;
  JumpPoint *jump_points_;
  size_t jump_point_size_;
  int loaded_language;           //language loaded in program (bf or BIO)
} InterpreterArguments;


//------------------------------------------------------------------------------
///
/// This struct holds the command line arguments
//
typedef struct
{
  int e_;                        //-e argument (execute)
  char *path;                    //path behind the -e argument
  int b_;                        //-b argument (bonus)
  int r_;                        //-r argument (reverse step)
} CommandLineArguments;


//------------------------------------------------------------------------------
///
/// This struct represents a stack that is used in BIO to remember the position
/// of opening brackets in the loaded program
//
typedef struct Loop_
{
  char *data_ptr_;
  struct Loop_ *next_;
  int cell_;
} Loop;


//------------------------------------------------------------------------------
///
/// This function allocates size memory and controls if there was an error and
/// reacts to it
///
/// @param size to allocate
///
/// @return void pointer to the allocated memory
//
void *saveMalloc(size_t size);


//------------------------------------------------------------------------------
///
/// This function allocates element * size memory, sets it to 0 and controls if
/// there was an error and reacts to it
///
/// @param element count of elements
/// @param size of one element
///
/// @return void pointer to the allocated memory
//
void *saveCalloc(size_t element_count, size_t size);


//------------------------------------------------------------------------------
///
/// This function reallocates a memory and controls if there was an error and
/// reacts to it
///
/// @param pointer to the memory that should be resized
/// @param size to reallocate
///
/// @return void pointer to the allocated memory
//
void *saveRealloc(void *pointer, size_t size);


//------------------------------------------------------------------------------
///
/// This function frees the memory the pointer is pointing to and sets it NULL
///
/// @param pointer to a pointer
//
void freePointer(void **pointer);


//------------------------------------------------------------------------------
///
/// This function free the memory of the double pointer and sets it NULL
///
/// @param pointer to a double pointer
//
void freeDoublePointer(void ***pointer);


//------------------------------------------------------------------------------
///
/// This function parses the command line arguments in argc and argv and fills
/// the CommandLineArguments struct
///
/// @param command_line_arguments pointer to the struct to fill
/// @param argc the argc argument from the main
/// @param argv the argv argument from the main
//
void parseCommandLineArguments(CommandLineArguments *command_line_arguments,
                               int argc, char *argv[]);


//------------------------------------------------------------------------------
///
/// This function prints the error string for the wrong used program and exits
/// with the corresponding error code
//
void exitWrongUsage();


//------------------------------------------------------------------------------
///
/// This function is called when the -e argument was passed. It loads the file
/// runs it and returns.
///
/// @param arguments pointer to the interpreter arguments
/// @param command_line_arguments pointer to the command line arguments
//
int runOnce(InterpreterArguments *arguments,
            CommandLineArguments *command_line_arguments);


//------------------------------------------------------------------------------
///
/// This function implements the debug mode
///
/// @param arguments pointer to the interpreter arguments
/// @param command_line_arguments pointer to the command line arguments
//
void debugger(InterpreterArguments *arguments,
             CommandLineArguments *command_line_arguments);


//------------------------------------------------------------------------------
///
/// This function loads a file with brainfuck or bio code
///
/// @param arguments pointer to the interpreter arguments
/// @param command_line_arguments pointer to the command line arguments
/// @param program_status pointer to the program status variable
/// @param data_segment_availability pointer to the data segment availability
///        variable
//
void load(InterpreterArguments *arguments,
         CommandLineArguments *command_line_arguments, int *program_status,
         int *data_segment_availability);


//------------------------------------------------------------------------------
///
/// This function runs mainly calls the brainfuck or bio interpreter
///
/// @param arguments pointer to the interpreter arguments
/// @param program_status pointer to the program status variable
/// @param data_segment_availability pointer to the data segment availability
///        variable
//
void run(InterpreterArguments *arguments, int *program_status,
        int *data_segment_availability);


//------------------------------------------------------------------------------
///
/// This function gets the Breakpoints and writes them in an array
///
/// @param program_loaded variable to check if a program is loaded
/// @param arguments the interpreter arguments (including the breakpoint-array)
/// @param breakpoints_size size of the breakpoint-array
//
void setBreakpoint(int program_loaded, InterpreterArguments *arguments);


//------------------------------------------------------------------------------
///
/// This function compares two numbers. It is used by qsort.
/// 
/// @param pointer1 pointer to the first number
/// @param pointer2 pointer to the second number
///
/// @return < 0 if number 1 is bigger than number 2
///         = 0 if number 1 is equal to number 2
///         > 0 if number 1 is smaller than number 2
//
int compareFunction(const void *pointer1, const void *pointer2);


//------------------------------------------------------------------------------
///
/// This function runs the interpreter with the entered number of steps
///
/// @param program_loaded variable to check if the program is loaded
/// @param arguments all the interpreter arguments
/// @param data_segment_availability pointer to the data segment availability
///        variable
///
/// @return program status
//
int step(int program_loaded, InterpreterArguments *arguments,
         int *data_segment_availability);


//------------------------------------------------------------------------------
///
/// This function prints the value of a data cell in the wanted format
///
/// @param program_loaded variable to check if the program is loaded
/// @param data_segment points at the beginning of the data array
/// @param data_pointer_ points at the present data cell
//
void memory(int data_segment_availability, unsigned char *data_segment,
            unsigned char *data_pointer_);


//------------------------------------------------------------------------------
///
/// This function prints the bf-instruction at the wanted position
///
/// @param program_loaded variable to check if the program is loaded
/// @param program_counter points at the present position in the program array
//
void show(int program_loaded, char *program_counter);


//------------------------------------------------------------------------------
///
/// This function writes new user entered values into the data array
///
/// @param program_loaded variable to check if the program is loaded
/// @param data_segment points at the beginning of the data array
/// @param data_pointer_ points at the present data cell
//
void change(int data_segment_availability, unsigned char *data_segment,
            unsigned char *data_pointer);


//------------------------------------------------------------------------------
///
/// Converting a given decimal number into a binary number
///
/// @param number number which should be converted
/// @param binary_number array where the converted number is written
/// @param digits number of digits the binary number will have
//
void binary(unsigned char number, char *binary_number, int digits);


//------------------------------------------------------------------------------
///
/// Loading the code character by character out of a file and write it into an
/// array.
///
/// @param *filedirectory directory of the codefile to fetch
/// @param *program Array in which the bf-code gets written
///
/// @return 0 when not loaded (error), LOADED_FROM_FILE when loaded
//
int loadBrainfuck(char *filedirectory,
                  InterpreterArguments *interpreter_arguments,
                  int bonus);


//------------------------------------------------------------------------------
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
void eval(InterpreterArguments *arguments, char *input_bfstring, int bonus,
          int *program_status, int *data_segment_availability);


//------------------------------------------------------------------------------
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
int checkCode(InterpreterArguments *interpreter_arguments, int character,
              int *position, int bonus);


//------------------------------------------------------------------------------
///
/// Interprets and runs the program code in the program array
///
/// @param interpreter_arguments pointer to an InterpreterArguments struct with
///                              all the arguments
///
/// @return  0 if the program ran to the end
///         -1 if the program ran the specified steps
///         -2 if the program stopped because of a breakpoint
//
int interpreterBrainfuck(InterpreterArguments *interpreter_arguments);


//------------------------------------------------------------------------------
///
/// Checks if the program counter is on a breakpoint
///
/// @param interpreter_arguments pointer to an InterpreterArguments struct with
///                              all the arguments
///
/// @return  0 if it is not on a breakpoint
///          1 if it is on a breakpoint
//
int onBreakpoint(InterpreterArguments *interpreter_arguments);


//------------------------------------------------------------------------------
///
/// This function returns an InterpreterArguments struct
/// For the data_segment are allocated 1024 bytes
///
/// If you want to reuse the data_segment with another code e.g. for the eval
/// command you can pass the three variables
///
/// @param data_segment pointer to the data segment to use
/// @param data_length pointer to the length of the data_segment
/// @param data_pointer pointer to the data pointer of the bf program
///
/// @return the InterpreterArguments struct
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


//------------------------------------------------------------------------------
///
/// This function frees all allocated memory in the InterpreterArguments struct
///
/// @param pointer to the InterpreterArguments struct
//
void freeInterpreterArguments(InterpreterArguments *interpreter_arguments);


//------------------------------------------------------------------------------
///
/// Expands the data segment by twice the size and sets the new memory to 0
///
/// @param data_segment double pointer to the data segment
/// @param data_length current size of the data segment
//
void expandDataSegment(unsigned char **data_segment, size_t *data_length,
                       unsigned char **data_pointer);


//------------------------------------------------------------------------------
///
/// Processes the user input ',' brainfuck command.
/// If the direction is positive it reads a character from the user.
/// If the direction is negative (reverse step) it searches the
/// overwritten_data_bytes array and inserts this byte in the data segment
///
/// @param interpreter_arguments Pointer to the interpreter arguments
/// @param direction Direction the program is running
//
void processUserInput(InterpreterArguments *interpreter_arguments,
                      int direction);


//------------------------------------------------------------------------------
///
/// Allocates and fills the jump cache
///
/// @param interpreter_arguments Pointer to the interpreter arguments
//
void createJumpCache(InterpreterArguments *interpreter_arguments);


//------------------------------------------------------------------------------
///
/// Processes a brainfuck loop
/// If the direction is positive it checks the condition, if it should jump it
/// calls searchMatchingBrace
/// If the direction is negative it searches for the jump in the jump array
/// and jumps backward if it finds a jump
///
/// @param interpreter_arguments all the interpreter arguments
/// @param direction the program runs
//
void processLoop(InterpreterArguments *interpreter_arguments, int direction);


//------------------------------------------------------------------------------
///
/// This function jumps to the corresponding bracket in a loop
///
/// @param interpreter_arguments all the interpreter arguments
//
int searchMatchingBrace(char *program_iterator);


//------------------------------------------------------------------------------
///
/// This function inserts a jump in the jump array and expands it if necessary
///
/// @param jumps pointer to the MadeJumps struct
/// @param jump the jump to insert
//
void insertMadeJump(MadeJumps *jumps, MadeJump jump);


//------------------------------------------------------------------------------
///
/// This function inserts a input in the input array and expands it if necessary
///
/// @param inputs pointer to the OverwrittenDataBytes struct
/// @param input the input to insert
//
void insertOverwrittenDataByte(OverwrittenDataBytes *inputs,
                               OverwrittenDataByte input);

//------------------------------------------------------------------------------
///
/// This function writes a new JumpPoint into the JumpPoint-Array
/// 
/// @param interpreter_arguments all the interpreter arguments
/// @param jump_point position of the edited jumppoint in the array 
//
void newJumpPoint(InterpreterArguments *interpreter_arguments, int jump_point);


//------------------------------------------------------------------------------
///
/// This function checks if there is already a JumpPoint where the id equals the
/// actual cell value
///
/// @param interpreter_arguments all the interpreter arguments
///
/// @return -1 if no JumpPoint with the given ID exists
/// @return the position of the JumpPoint in the JumpPoint-Array
//
int checkForExistingJumpPoint(InterpreterArguments *arguments);


//------------------------------------------------------------------------------
///
/// Interpreter for BIO
///
/// @param interpreter_arguments all the interpreter arguments
///
/// @return TODO
//
int interpreterBio(InterpreterArguments *interpreter_arguments);


//------------------------------------------------------------------------------
///
/// Loading the code character by character out of a file and write it into an
/// array.
///
/// @param filedirectory directory of the codefile to fetch
/// @param interpreter_arguments all the interpreter arguments
///
/// @return 0 when not loaded (error), LOADED_FROM_FILE when loaded
//
int loadBio(char *file_directory, InterpreterArguments *arguments);


//------------------------------------------------------------------------------
///
/// Gets the next command? TODO
///
/// @param program_ctr
/// @param cell
///
/// @return
//
int getNextCmd(char **program_ctr, int *cell);


//------------------------------------------------------------------------------
///
/// Pushes to the loop stack? TODO
///
/// @param top
/// @param data_ptr
/// @param cell
//
void push(Loop **top, char *data_ptr, int *cell);


//------------------------------------------------------------------------------
///
/// Pops from the loop stack? TODO
///
/// @param top
///
/// @return
//
char *pop(Loop **top);


//------------------------------------------------------------------------------
///
/// The main program.
/// Uses to given input to print it in reversed order to the console. TODO
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

  CommandLineArguments command_line_arguments = {0, NULL, 0, 0};

  parseCommandLineArguments(&command_line_arguments, argc, argv);

  if (command_line_arguments.e_ == 1)
  {
    return runOnce(&arguments, &command_line_arguments);
  }

  debugger(&arguments, &command_line_arguments);

  return EXIT_SUCCESS;
}


void *saveMalloc(size_t size)
{
  void *pointer = malloc(size);
  if (pointer == NULL)
  {
    printf("[ERR] out of memory\n");
    exit(OUT_OF_MEMORY_RETURN_CODE);
  }
  return pointer;
}


void *saveCalloc(size_t member_count, size_t size)
{
  void *pointer = calloc(member_count, size);
  if (pointer == NULL)
  {
    printf("[ERR] out of memory\n");
    exit(OUT_OF_MEMORY_RETURN_CODE);
  }
  return pointer;
}


void *saveRealloc(void *pointer, size_t size)
{
  pointer = realloc(pointer, size);
  if (pointer == NULL)
  {
    printf("[ERR] out of memory\n");
    exit(OUT_OF_MEMORY_RETURN_CODE);
  }
  return pointer;
}


void freePointer(void **pointer)
{
  if (*pointer != NULL)
  {
    free(*pointer);
    *pointer = NULL;
  }
}


void freeDoublePointer(void ***pointer)
{
  if (*pointer != NULL)
  {
    freePointer(*pointer);

    free(*pointer);
    *pointer = NULL;
  }
}


void parseCommandLineArguments(CommandLineArguments *command_line_arguments,
                               int argc, char *argv[])
{
  if (argc >= 2)
  {
    int argument;
    for (argument = 1; argument < argc; argument++)
    {
      if (strcmp(argv[argument], "-b") == 0)
      {
        command_line_arguments->b_ = 1;
      }
      else if (strcmp(argv[argument], "-e") == 0)
      {
        if (argc >= argument + 2)
        {
          command_line_arguments->e_ = 1;
          argument++;
          command_line_arguments->path = argv[argument];
        }
        else
        {
          exitWrongUsage();
        }
      }
      else if (strcmp(argv[argument], "-r") == 0)
      {
        command_line_arguments->r_ = 1;
      }
      else
      {
        exitWrongUsage();
      }
    }
  }

  if (command_line_arguments->r_ == 1 && command_line_arguments->b_ == 0)
  {
    command_line_arguments->r_ = 0;
  }
}


void exitWrongUsage()
{
  printf("[ERR] usage: ./assa [-e brainfuck_filnename]\n");
  exit(WRONG_USAGE_RETURN_CODE);
}


void debugger(InterpreterArguments *arguments,
             CommandLineArguments *command_line_arguments)
{
  int program_status = NOT_LOADED;
  int data_segment_availability = !DATA_SEGMENT_AVAILABLE;
  // allocate char array for console input
  size_t line_size = 100;
  char *line = saveCalloc(line_size, sizeof(char));

  InterpreterArguments evalArguments = getUsableInterpreterArgumentsStruct(
    arguments->data_segment_, arguments->data_length_, arguments->data_pointer_
  );

  if (command_line_arguments->r_ == 1)
  {
    arguments->activate_reverse_step_ = 1;
  }

  // print first command line line output
  printf("esp> ");
  while ((fgets(line, (int) line_size, stdin) || 1)
         && !ferror(stdin) && !feof(stdin)
         && strcmp(line, "quit\n"))
  {
    // fgets adds at the end '\n\0'. Therefore override '\n' with '\0'
    line[strlen(line) - 1] = '\0';

    char *cmd = strtok(line, "  ");
    if (cmd)
    {
      if (strcmp(cmd, "load") == 0)
      {
        load(arguments, command_line_arguments, &program_status,
             &data_segment_availability);
      }
      else if (strcmp(cmd, "run") == 0)
      {
        run(arguments, &program_status, &data_segment_availability);
      }
      else if (strcmp(cmd, "eval") == 0)
      {
        char *bfstring = strtok(NULL, " ");
        eval(&evalArguments, bfstring, command_line_arguments->b_,
             &program_status, &data_segment_availability);
      }
      else if (strcmp(cmd, "break") == 0)
      {
        setBreakpoint(program_status, arguments);
      }
      else if (strcmp(cmd, "step") == 0)
      {
        program_status = step(program_status, arguments,
                              &data_segment_availability);
      }
      else if (strcmp(cmd, "memory") == 0)
      {
        memory(data_segment_availability, *(arguments->data_segment_),
               *(arguments->data_pointer_));
      }
      else if (strcmp(cmd, "show") == 0)
      {
        show(program_status, arguments->program_counter_);
      }
      else if (strcmp(cmd, "change") == 0)
      {
        change(data_segment_availability, *(arguments->data_segment_),
               *(arguments->data_pointer_));
      }
    }

    // print command line line output
    printf("esp> ");
  }

  freePointer((void **) &line);

  freeInterpreterArguments(arguments);

  // set this pointers to NULL so that they are not freed a second time
  evalArguments.data_segment_ = NULL;
  evalArguments.data_pointer_ = NULL;
  evalArguments.data_length_ = NULL;
  freeInterpreterArguments(&evalArguments);

  if (!feof(stdin))
    printf("Bye.\n");
}


int runOnce(InterpreterArguments *arguments,
            CommandLineArguments *command_line_arguments)
{
  int return_code = loadBrainfuck(command_line_arguments->path, arguments,
                                  command_line_arguments->b_);

  if (return_code == FILE_READ_ERROR)
  {
    return FILE_READ_ERROR_RETURN_CODE;
  }
  if (return_code == FILE_PARSE_ERROR)
  {
    return FILE_PARS_ERROR_RETURN_CODE;
  }

  interpreterBrainfuck(arguments);

  freeInterpreterArguments(arguments);

  return EXIT_SUCCESS;
}


void load(InterpreterArguments *arguments,
         CommandLineArguments *command_line_arguments, int *program_status,
         int *data_segment_availability)
{
  // get path to file
  char *cmd = strtok(NULL, " ");
  if (cmd != NULL)
  {
    // get a pointer to the first occurrence of '.'
    char *ext = strrchr(cmd, '.');
    if (command_line_arguments->b_ && ext)
    {
      if (strcmp(ext, ".bio") == 0)
      {
        arguments->loaded_language = 1;
      }
      else // can be .bf or anything else to use bf input language
      {
        arguments->loaded_language = 0;
      }
    }
    else // no file extension or bonus not set, use bf as default
    {
      arguments->loaded_language = 0;
    }
    int return_code = 0;
    if (arguments->loaded_language == 0)
    {
      return_code = loadBrainfuck(cmd, arguments,
                                  command_line_arguments->b_);
    }
    else if (arguments->loaded_language == 1)
    {
      return_code = loadBio(cmd, arguments);
    }

    if (return_code == SUCCESS)
    {
      *program_status = LOADED_FROM_FILE;
      *data_segment_availability = !DATA_SEGMENT_AVAILABLE;
    }
    else
    {
      *program_status = NOT_LOADED;
    }

  }
  else
  {
    printf("[ERR] wrong parameter count\n");
  }
}


void run(InterpreterArguments *arguments, int *program_status,
        int *data_segment_availability)
{
  if (*program_status != LOADED_FROM_FILE)
  {
    printf("[ERR] no program loaded\n");
  }
  else
  {
    int stop_reason = -1;
    if (arguments->loaded_language == 0)
    {
      stop_reason = interpreterBrainfuck(arguments);
    }
    else if (arguments->loaded_language == 1)
    {
      stop_reason = interpreterBio(arguments);
    }

    if (stop_reason == REGULAR_STOP) // ran to the end
    {
      *program_status = RUN_FINISHED;
    }
    else if (stop_reason == STEP_STOP)
    {
      //should never be reached
    }
    else if (stop_reason == BREAKPOINT_STOP) // stopped at breakpoint
    {
      // does nothing anymore because the breakpoint is cleared in the
      // interpreter
    }

    *data_segment_availability = DATA_SEGMENT_AVAILABLE;
  }
}


void setBreakpoint(int program_loaded, InterpreterArguments *arguments)
{
  if (program_loaded != LOADED_FROM_FILE)
  {
    printf("[ERR] no program loaded\n");
    return;
  }
  // get breakpoint position
  char *number_input = strtok(NULL, " ");
  if (number_input == NULL)
  {
    printf("[ERR] wrong parameter count\n");
    return;
  }

  int number = (int) strtol(number_input, (char **) NULL, 10);

  // allow only positive breakpoints
  if (number < 0)
  {
    return;
  }

  // if breakpoint with same number is already there, don't add it
  int breakp = 0;
  for (breakp = 0; breakp < arguments->breakpoints_.count_; breakp++)
  {
    if (arguments->breakpoints_.array_[breakp] == number)
    {
      return;
    }
  }

  // extend breakpoints memory if needed
  if (arguments->breakpoints_.count_ + 1 >
          arguments->breakpoints_.allocated_memory_)
  {
    arguments->breakpoints_.allocated_memory_ *= 2;
    arguments->breakpoints_.array_ = saveRealloc(arguments->breakpoints_.array_,
                                      arguments->breakpoints_.allocated_memory_
                                      * sizeof(int));
  }

  // add next breakpoint
  arguments->breakpoints_.array_[arguments->breakpoints_.count_] = number;
  arguments->breakpoints_.count_++;

  // sort breakpoints ascending (eg. 3 - 7 - 10)
  qsort(arguments->breakpoints_.array_, arguments->breakpoints_.count_,
        sizeof(int), compareFunction);
  return;
}


int compareFunction(const void *pointer1, const void *pointer2)
{
  return (*(int *) pointer1 - *(int *) pointer2);
}


int step(int program_loaded, InterpreterArguments *arguments,
         int *data_segment_availability)
{
  if (program_loaded != LOADED_FROM_FILE)
  {
    printf("[ERR] no program loaded\n");
    return program_loaded;
  }
  char *steps = strtok(NULL, " ");
  if (steps == NULL)
    arguments->steps_ = 1;
  else
    arguments->steps_ = strtol(steps, (char **) NULL, 10);

  if (arguments->steps_ == 0 ||
     (arguments->steps_ < 0 && arguments->activate_reverse_step_ == 0))
  {
    // don't call the interpreter with 0 steps because it would run indefinitely
    // but just to nothing.
    arguments->steps_ = 0;  //set it 0 if it is < 0
    return program_loaded;
  }

  int stop_reason = interpreterBrainfuck(arguments);

  *data_segment_availability = DATA_SEGMENT_AVAILABLE;

  if (stop_reason == REGULAR_STOP)
    return RUN_FINISHED;

  arguments->steps_ = 0;

  return program_loaded;
}


void memory(int data_segment_availability, unsigned char *data_segment,
            unsigned char *data_pointer_)
{
  if (data_segment_availability == !DATA_SEGMENT_AVAILABLE)
  {
    printf("[ERR] no program loaded\n");
    return;
  }
  //if no format is given Hex is default
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
  int size = (size_input != NULL) ?
             (int) strtol(size_input, (char **) NULL, 10) : 10;

  // Source: http://stackoverflow
  // .com/questions/4214314/get-a-substring-of-a-char
  // print "size" characters from target string
  // at code position + offset
  printf("%.*s\n", size, program_counter);
}


void change(int data_segment_availability, unsigned char *data_segment,
            unsigned char *data_pointer)
{
  if (data_segment_availability == !DATA_SEGMENT_AVAILABLE)
  {
    printf("[ERR] no program loaded\n");
    return;
  }

  char *number_input = strtok(NULL, " ");
  if (number_input == NULL)
  {
    *data_pointer = 0;
    return;
  }
  char *end_ptr;
  int number = (int) strtol(number_input, &end_ptr, 10);


  char *hex_byte_input = strtok(NULL, " ");
  if (hex_byte_input == NULL)
  {
    data_segment[number] = 0;
    return;
  }
  int hex_byte = (int) strtol(hex_byte_input, &end_ptr, 16);
  data_segment[number] = (unsigned char) hex_byte;
}


void binary(unsigned char number, char *binary_number, int digits)
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


int loadBrainfuck(char *file_directory, InterpreterArguments *arguments,
                  int bonus)
{
  resetInterpreterArguments(arguments);
  FILE *file;

  if ((file = fopen(file_directory, "r")) == NULL)
  {
    printf("[ERR] reading the file failed\n");  //check if the file can be read
    return FILE_READ_ERROR;
  }

  int character; // has to be a integer to read EOF as -1
  int bracket_counter = 0;
  size_t program_size = 1024;
  int position = 0;
  arguments->program_ = saveMalloc(program_size);

  //read the file char by char and write it into an array
  while ((character = fgetc(file)) != EOF)
  {
    //is 0 if #opened brackets == #closed brackets
    bracket_counter += checkCode(arguments, character, &position, bonus);

    //resizes the programmArray if the file is too long
    if (position == program_size - 1)
    {
      program_size *= 2;
      arguments->program_ = saveRealloc(arguments->program_, program_size);
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

  fclose(file);

  return SUCCESS;
}


void eval(InterpreterArguments *arguments, char *input_bfstring, int bonus,
          int *program_status, int *data_segment_availability)
{
  int bracket_counter = 0;
  int position = 0;
  size_t len = strlen(input_bfstring);

  // specification: maxlength of eval input: 80 chars
  if (len > 80)
  {
    len = 80;
  }

  arguments->program_length_ = len + 1;
  arguments->program_ = saveMalloc(arguments->program_length_ * sizeof(char));

  //set program_counter_ to the beginning of the code
  arguments->program_counter_ = arguments->program_;

  int loopCounter;  //just for the for-loop
  for (loopCounter = 0; loopCounter < len; loopCounter++)
  {
    bracket_counter +=
      checkCode(arguments, input_bfstring[loopCounter], &position, bonus);
  }
  arguments->program_[position] = '\0';  //end of string

  //if #opened brackets is not #closed brackets
  if (bracket_counter != 0)
    printf("[ERR] parsing of input failed\n");
  else
  {
    interpreterBrainfuck(arguments);
    freePointer((void **) &arguments->program_);

    //only set the program_status variable if no program was loaded yet
    if (*program_status == NOT_LOADED)
      *program_status = LOADED_FROM_EVAL;

    *data_segment_availability = DATA_SEGMENT_AVAILABLE;
  }
}


int checkCode(InterpreterArguments *arguments, int character, int *position,
              int bonus)
{
  if (character == '<' || character == '>' || character == '+' ||
      character == '-' || character == '.' || character == ',' ||
      character == '[' || character == ']')
  {
    arguments->program_[*position] = (char) character;
    (*position)++;
  }
  else if ((bonus && character == '&') || (bonus && character == '%'))
  {
    arguments->program_[*position] = (char) character;
    (*position)++;
  }

  if (character == '[')
    return 1;
  else if (character == ']')
    return -1;
  else
    return 0;
}


/*
War mal eine Idee

#define COMMAND_MOVE_LEFT 0
#define COMMAND_MOVE_RIGHT 1
#define COMMAND_INCREMENT 2
#define COMMAND_DECREMENT 3
#define COMMAND_LOOP_START 4
#define COMMAND_LOOP_END 5
#define COMMAND_PRINT 6
#define COMMAND_INPUT 7
#define COMMAND_SET_JUMP_POINT 8
#define COMMAND_JUMP 9

const char command[10][2] = {
  {'>','<'},
  {'<','>'},
  {'+','-'},
  {'-','+'},
  {'[',']'},
  {']','['},
  {'.','.'},
  {',',','},
  {'&','&'},
  {'%','%'}
};*/

int interpreterBrainfuck(InterpreterArguments *interpreter_arguments)
{
  // non static variables
  const int direction = (interpreter_arguments->steps_ < 0) ? -1 : 1;

  const char move_forward = (direction == 1) ? (char) '>' : (char) '<';
  const char move_back = (direction == 1) ? (char) '<' : (char) '>';
  const char increment = (direction == 1) ? (char) '+' : (char) '-';
  const char decrement = (direction == 1) ? (char) '-' : (char) '+';
  // print and read do not change
  const char loop_start = (direction == 1) ? (char) '[' : (char) ']';
  const char loop_end = (direction == 1) ? (char) ']' : (char) '[';

  const char jump_point = (char) '&';
  const char jump = (char) '%';

  int on_breakpoint = 0;
  int jump_point_nr = 0;

  //create a local variable for steps because it should not be changed.
  //if -1 runs infinitely long (to the end of the program)
  long long steps = (interpreter_arguments->steps_ == 0) ?
                    -1 : interpreter_arguments->steps_;

  //if running reverse the program must run shifted one to the left,
  //increment at the end
  if (direction == -1)
    interpreter_arguments->program_counter_--;

  if (interpreter_arguments->jump_cache_ == NULL)
    createJumpCache(interpreter_arguments);

#ifdef __BENCHMARK__
  clock_t start = clock();
#endif

  for (; *(interpreter_arguments->program_counter_) != 0 && steps != 0;
         steps -= direction)
  {
    if ((on_breakpoint = onBreakpoint(interpreter_arguments)))
      break;

    // ------------  >  ------------
    if (*(interpreter_arguments->program_counter_) == move_forward)
    {
      // check it the data pointer is out of the data segment
      if (++(*interpreter_arguments->data_pointer_) >
          *interpreter_arguments->data_segment_ +
          *(interpreter_arguments->data_length_) - 1)
      {
        // if so expand the data segment
        expandDataSegment(interpreter_arguments->data_segment_,
                          interpreter_arguments->data_length_,
                          interpreter_arguments->data_pointer_);
      }
      interpreter_arguments->program_counter_ += direction;
    }


      // ------------  <  ------------
    else if (*(interpreter_arguments->program_counter_) == move_back)
    {
      // check if the data pointer gets to small
      if (--(*interpreter_arguments->data_pointer_) <
          *interpreter_arguments->data_segment_)
      {
        // if so don't let it
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


      // ------------  ,  ------------
    else if (*(interpreter_arguments->program_counter_) == ',')
    {
      processUserInput(interpreter_arguments, direction);
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

      if (interpreter_arguments->jump_points_ != NULL)
      {
        if (checkForExistingJumpPoint(interpreter_arguments) != -1)
          jump_point_nr = checkForExistingJumpPoint(interpreter_arguments);
        else
          jump_point_nr = (int) interpreter_arguments->jump_point_size_;
      }
      newJumpPoint(interpreter_arguments, jump_point_nr);

      interpreter_arguments->program_counter_ += direction;
    }


      // ------------  %  ------------
    else if (*(interpreter_arguments->program_counter_) == jump)
    {
      if (checkForExistingJumpPoint(interpreter_arguments) != -1)
      {
        jump_point_nr = checkForExistingJumpPoint(interpreter_arguments);
        interpreter_arguments->program_counter_ =
          interpreter_arguments->jump_points_[jump_point_nr].address_;
      }

      interpreter_arguments->program_counter_ += direction;
    }

    interpreter_arguments->step_counter_ += direction;
  }

#ifdef __BENCHMARK__
  printf("\n\nCLOCKS: %llu\n", (unsigned long long) (clock() - start));
#endif


  if (direction == -1)
    interpreter_arguments->program_counter_++;

  if (steps == 0) //program ran the specified steps to the end
  {
    return STEP_STOP;
  }

  if (on_breakpoint)
  {
    return BREAKPOINT_STOP;
  }

  return REGULAR_STOP;
}


int onBreakpoint(InterpreterArguments *interpreter_arguments)
{
  int *breakpoint;
  for (breakpoint = interpreter_arguments->breakpoints_.array_;
       breakpoint < interpreter_arguments->breakpoints_.array_ +
                    interpreter_arguments->breakpoints_.count_; breakpoint++)
  {
    if (interpreter_arguments->program_ + *breakpoint ==
        interpreter_arguments->program_counter_)
    {
      // remove the breakpoint from the array by moving the other breakpoints
      // over it
      memmove(interpreter_arguments->breakpoints_.array_, breakpoint + 1,
              (interpreter_arguments->breakpoints_.count_ -
               (breakpoint - interpreter_arguments->breakpoints_.array_) - 1) *
              sizeof(int));

      interpreter_arguments->breakpoints_.count_--;

      return 1;
    }
  }
  return 0;
}


InterpreterArguments getUsableInterpreterArgumentsStruct(
  unsigned char **data_segment, size_t *data_length,
  unsigned char **data_pointer)
{
  InterpreterArguments interpreter_arguments = {
    NULL,         // program_
    0,            // program_length_
    NULL,         // jump_cache_
    data_segment, // data_segment_
    data_length,  // data_length_
    NULL,         // program_counter_
    data_pointer, // data_pointer_
    0,            // steps_
    {             // breakpoints_
      NULL,         // array_
      10,           // allocated_memory_
      0             // count_
    },
    0,            // step_counter_
    0,            // activate_reverse_step_
    {             // made_jumps_
      0,            // count_
      200,            // allocated_memory_
      NULL          // array_
    },
    {             // inputs
      0,            // count_
      20,            // allocated_memory_
      NULL          // array_
    },
    NULL,         // jump_points_
    1         // size_of_jump_points_
  };


  if (data_segment == NULL)
  {
    interpreter_arguments.data_length_ = saveMalloc(sizeof(size_t));
    *interpreter_arguments.data_length_ = 1024;

    interpreter_arguments.data_segment_ = saveMalloc(sizeof(unsigned char *));
    *interpreter_arguments.data_segment_ =
      saveCalloc(*interpreter_arguments.data_length_, sizeof(unsigned char));

    interpreter_arguments.data_pointer_ = saveMalloc(sizeof(unsigned char *));
    *interpreter_arguments.data_pointer_ = *interpreter_arguments.data_segment_;
  }

  interpreter_arguments.breakpoints_.array_ = (int *) saveMalloc(
    interpreter_arguments.breakpoints_.allocated_memory_ * sizeof(int));


  interpreter_arguments.made_jumps_.array_ = (MadeJump *) saveMalloc(
    interpreter_arguments.made_jumps_.allocated_memory_ * sizeof(MadeJump));


  interpreter_arguments.overwrittenDataBytes_.array_ = (OverwrittenDataByte *)
    saveMalloc(interpreter_arguments.overwrittenDataBytes_.allocated_memory_ *
               sizeof(OverwrittenDataByte));

  return interpreter_arguments;
}


void resetInterpreterArguments(InterpreterArguments *interpreter_arguments)
{
  freePointer((void **) &(interpreter_arguments->program_));
  freePointer((void **) &(interpreter_arguments->jump_cache_));

  interpreter_arguments->step_counter_ = 0;
  interpreter_arguments->made_jumps_.count_ = 0;

  memset(*interpreter_arguments->data_segment_, 0,
         *interpreter_arguments->data_length_ * sizeof(unsigned char));

  *interpreter_arguments->data_pointer_ = *interpreter_arguments->data_segment_;

  interpreter_arguments->breakpoints_.count_ = 0;
}


void freeInterpreterArguments(InterpreterArguments *interpreter_arguments)
{
  freePointer((void **) &(interpreter_arguments->program_));
  freePointer((void **) &(interpreter_arguments->jump_cache_));
  freeDoublePointer((void ***) &(interpreter_arguments->data_segment_));
  freePointer((void **) &(interpreter_arguments->data_length_));
  freePointer((void **) &(interpreter_arguments->data_pointer_));
  freePointer((void **) &(interpreter_arguments->breakpoints_));
  freePointer((void **) &(interpreter_arguments->made_jumps_.array_));
  freePointer((void **) &(interpreter_arguments->overwrittenDataBytes_.array_));
  freePointer((void **) &(interpreter_arguments->jump_points_));
}


void expandDataSegment(unsigned char **data_segment, size_t *data_length,
                       unsigned char **data_pointer)
{
  unsigned long data_pointer_offset = *data_pointer - *data_segment;

  *data_length *= 2;
  *data_segment = saveRealloc(*data_segment, *data_length);

  *data_pointer = *data_segment + data_pointer_offset;

  //set the new memory to 0
  memset(*data_segment + (*data_length) / 2, 0, (*data_length) / 2);
}


void processUserInput(InterpreterArguments *interpreter_arguments,
                      int direction)
{
  if (direction == 1)
  {
    if (interpreter_arguments->activate_reverse_step_ == 1)
    {
      insertOverwrittenDataByte(&(interpreter_arguments->overwrittenDataBytes_),
                                (OverwrittenDataByte) {
                                  interpreter_arguments->step_counter_,
                                  **interpreter_arguments->data_pointer_
                                });
    }

    **interpreter_arguments->data_pointer_ = (unsigned char) getchar();

    interpreter_arguments->program_counter_++;
  }
  else if (direction == -1)
  {
    //search in the jumps array if the program was jumping
    int search_loop_index;

    //search from back because, the chance that the overwrittenDataByte is at
    // the end is high
    for (search_loop_index =
           interpreter_arguments->overwrittenDataBytes_.count_ - 1;
         search_loop_index >= 0; search_loop_index--)
    {
      //if this is the correct value
      if (interpreter_arguments->overwrittenDataBytes_.
        array_[search_loop_index].step_ ==
          interpreter_arguments->step_counter_ - 1)
      {
        //set it
        **interpreter_arguments->data_pointer_ =
          interpreter_arguments->overwrittenDataBytes_.
            array_[search_loop_index].value_;
      }
    }

    interpreter_arguments->program_counter_--;
  }
}


void createJumpCache(InterpreterArguments *interpreter_arguments)
{
  char *program_iterator = interpreter_arguments->program_;

  // calloc so that the program does not jump somewhere when there is an false
  // access
  interpreter_arguments->jump_cache_ = saveCalloc(
    (strlen(interpreter_arguments->program_)), sizeof(int));

  for (; *program_iterator != 0; program_iterator++)
  {
    if (*program_iterator == '[')
    {
      int jump_distance = searchMatchingBrace(program_iterator);

      unsigned int program_counter =
        (unsigned int) (program_iterator - interpreter_arguments->program_);

      // insert the jump distance for the [ in the cache
      interpreter_arguments->jump_cache_[program_counter] = jump_distance - 1;

      // insert the jump distance for the corresponding ] in the cache
      interpreter_arguments->jump_cache_[program_counter + jump_distance - 1] =
        -(jump_distance - 1);
    }
  }
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
      int distance = interpreter_arguments->jump_cache_
      [interpreter_arguments->program_counter_ -
       interpreter_arguments->program_];

      interpreter_arguments->program_counter_ += distance;

      if (interpreter_arguments->activate_reverse_step_ == 1)
      {
        insertMadeJump(&interpreter_arguments->made_jumps_,
                       (MadeJump) {interpreter_arguments->step_counter_,
                                   distance});
      }
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
    for (search_loop_index = interpreter_arguments->made_jumps_.count_;
         search_loop_index > 0; search_loop_index--)
    {
      //if the program was jumping
      if (interpreter_arguments->made_jumps_.array_[search_loop_index].step_ ==
          interpreter_arguments->step_counter_ - 1)
      {
        //jump in the opposite direction
        interpreter_arguments->program_counter_ -=
          interpreter_arguments->made_jumps_.array_[search_loop_index].distance_;

        jump_found = 1;
      }
    }

    if (jump_found == 0)
      interpreter_arguments->program_counter_--;
  }
}


int searchMatchingBrace(char *program_iterator)
{
  int loop_counter = 1;
  int distance = 0;

  distance++;
  program_iterator++;

  while (loop_counter > 0 && *program_iterator != 0)
  {
    if (*program_iterator == '[')
    {
      loop_counter++;
    }
    else if (*program_iterator == ']')
    {
      loop_counter--;
    }

    distance++;
    program_iterator++;
  }

  return distance;
}


void insertMadeJump(MadeJumps *jumps, MadeJump jump)
{
  jumps->array_[jumps->count_] = jump;

  jumps->count_++;

  if (jumps->count_ == jumps->allocated_memory_)
  {
    //extend array
    jumps->allocated_memory_ *= 2;
    jumps->array_ = saveRealloc(jumps->array_,
                                jumps->allocated_memory_ * sizeof(MadeJump));
  }
}


void insertOverwrittenDataByte(OverwrittenDataBytes *overwrittenDataBytes,
                               OverwrittenDataByte overwrittenDataByte)
{
  overwrittenDataBytes->array_[overwrittenDataBytes->count_] =
    overwrittenDataByte;

  overwrittenDataBytes->count_++;

  if (overwrittenDataBytes->count_ == overwrittenDataBytes->allocated_memory_)
  {
    //extend array
    overwrittenDataBytes->allocated_memory_ *= 2;
    overwrittenDataBytes->array_ =
      saveRealloc(overwrittenDataBytes->array_,
                  overwrittenDataBytes->allocated_memory_ *
                  sizeof(OverwrittenDataByte));
  }
}


void newJumpPoint(InterpreterArguments *arguments, int jump_point)
{
  // allocate memory if it isn't already
  if (arguments->jump_points_ == NULL)
  {
    arguments->jump_points_ = saveMalloc(sizeof(JumpPoint));
  }
  // extend jumppoint memory if needed
  if (jump_point + 1 > arguments->jump_point_size_)
  {
    arguments->jump_point_size_++;
    arguments->jump_points_ = saveRealloc(arguments->jump_points_,
                                          arguments->jump_point_size_ *
                                          sizeof(JumpPoint));
  }

  // add next jumppoint
  arguments->jump_points_[jump_point].address_ = arguments->program_counter_;

  arguments->jump_points_[jump_point].id_ = **(arguments->data_pointer_);

}


int checkForExistingJumpPoint(InterpreterArguments *arguments)
{
  // run through the JumpPoint array and check for JumpPoint id
  int count_var = 0;
  for (count_var = 0; count_var <= arguments->jump_point_size_; count_var++)
  {
    if (arguments->jump_points_[count_var].id_ ==
        **(arguments->data_pointer_))
      return count_var;
  }
  return -1;
}


void push(Loop **top, char *data_ptr, int *cell)
{
  // make new stack item and copy data to it:
  Loop *new_item = saveMalloc(sizeof(Loop));
  new_item->data_ptr_ = data_ptr;
  new_item->cell_ = *cell;

  new_item->next_ = *top;    // next points to previous top
  *top = new_item;          // top now points to new item
}


char *pop(Loop **top)
{
  Loop *old_top = *top;       // remember the old top
  char *data_ptr = old_top->data_ptr_;   // remember the old data
  *top = old_top->next_;       // move top down
  free(old_top);              // now we can free the old StackItem
  return data_ptr;            // and return the data we remembered
}

// TODO: only reserve 3 chars in interpreter_arguments->data_segment_ for BIO (as is written in specs of BIO)
int interpreterBio(InterpreterArguments *interpreter_arguments)
{
  int *cell = saveCalloc(1, sizeof(int));
  Loop *loop_stack = NULL;

  for (; *(interpreter_arguments->program_counter_) != '\0';
         interpreter_arguments->program_counter_++)
  {
    if (*(interpreter_arguments->program_counter_ + 1) == '\0')
    {
      break;
    }
    int cmd = getNextCmd(&interpreter_arguments->program_counter_, cell);
    if (cmd == -1)
    {
      //failed
      return -1;
    }
    else if (cmd == 0)
    {
      (*(interpreter_arguments->data_segment_))[*cell]++;
    }
    else if (cmd == 1)
    {
      (*(interpreter_arguments->data_segment_))[*cell]--;
    }
    else if (cmd == 2)
    {
      if ((*(interpreter_arguments->data_segment_))[*cell] == 0)
      {
        // go to matching end bracket
        int found_right = 1;
        while (found_right != 0)
        {
          interpreter_arguments->program_counter_++;
          if (*(interpreter_arguments->program_counter_) == '}')
          {
            found_right--;
          }
          else if (*(interpreter_arguments->program_counter_) == '{')
          {
            found_right++;
          }
        }
        interpreter_arguments->program_counter_--;
      }

      // push start position of loop
      push(&loop_stack, interpreter_arguments->program_counter_, cell);
    }
    else if (cmd == 3)
    {
      if ((*(interpreter_arguments->data_segment_))[loop_stack->cell_] != 0)
      {
        if (loop_stack == NULL)
        {
          // TODO: do this in loadBio or make new return state for interpreter??
          // too much closing brackets
          return -1;
        }
        interpreter_arguments->program_counter_ = loop_stack->data_ptr_;
      }
      else
      {
        pop(&loop_stack);
      }
    }
    else if (cmd == 4)
    {
      putchar((*(interpreter_arguments->data_segment_))[*cell]);
    }
  }
  return REGULAR_STOP;
}


int getNextCmd(char **program_ctr, int *cell)
{
  int ret = -1;
  char first = **program_ctr;
  char second = *(*program_ctr + 1);

  if (first == '}' && second == ';')
  {
    (*program_ctr)++;
    return 3;
  }
  if (second == 'o')
  {
    if (first == '0')
    {
      ret = 0; // Increment the specified block
    }
    else if (first == '1')
    {
      ret = 1; // Decrement the specified block
    }
    // get the correct cell (either x -> 0, y -> 1, z -> 2)
    *cell = *(*program_ctr + 2) - 'x';
    if (0 <= *cell && *cell <= 2 && *(*program_ctr + 3) == ';')
    {
      //0ox;  0oy;  0oz;
      *program_ctr += 3;
    }
    else
    {
      ret = -1;
    }
  }
  else if (second == 'i')
  {
    if (first == '0')
    {
      ret = 2; // While the block is not 0
    }
    else if (first == '1')
    {
      ret = 4; // Output the block
    }
    // get the correct cell (either x -> 0, y -> 1, z -> 2)
    *cell = *(*program_ctr + 2) - 'x';
    if (0 <= *cell && *cell <= 2)
    {
      if (!((ret == 2 && *(*program_ctr + 3) == '{') ||
            (ret == 4 && *(*program_ctr + 3) == ';')))
      {
        ret = -1;
      }
      //0ox;  0oy;  0oz;  Oix{  etc.
      *program_ctr += 3;
    }
    else
    {
      ret = -1;
    }
  }
  return ret;
}


int loadBio(char *file_directory, InterpreterArguments *arguments)
{
  FILE *file;

  if ((file = fopen(file_directory, "r")) == NULL)
  {
    printf("[ERR] reading the file failed\n");
    return NOT_LOADED;
  }

  int character; // has to be a integer to read EOF as -1
  int position = 0;
  size_t program_size = 1024;
  arguments->program_ = saveMalloc(program_size);
  int comment = 0;

  //read the file char by char and write it into an array
  while ((character = fgetc(file)) != EOF)
  {
    if (comment == 1)
    {
      if (character == '\n')
      {
        //comment ended
        comment = 0;
      }
      continue;
    }
    if (character == '/')
    {
      if ((character = fgetc(file)) == EOF)
      {
        break;
      }
      else
      {
        if (character == '/')
        {
          // double "/" = comment
          comment = 1;
          continue;
        }
      }
    }
    if (character == '0' || character == '1' || character == 'o' ||
        character == 'i' || character == 'x' || character == 'y' ||
        character == 'z' || character == '{' || character == '}' ||
        character == ';')
    {
      // get more program memory if the file is too long
      if (position == program_size)
      {
        program_size *= 2;
        arguments->program_ = saveRealloc(arguments->program_, program_size);
      }

      arguments->program_[position++] = (char) character;
    }
  }
  arguments->program_[position] = '\0';  //end of string
  arguments->program_length_ = program_size * sizeof(char);
  //set program_counter_ to the beginning of the code
  arguments->program_counter_ = arguments->program_;

  if (arguments->program_[position - 1] != ';')
  {
    printf("[ERR] parsing of input failed\n");
    return FILE_PARSE_ERROR;
  }

  fclose(file);

  return SUCCESS;
}
