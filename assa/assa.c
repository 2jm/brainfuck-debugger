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

// TODO fibonacci.bf crashes

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


//------------------------------------------------------------------------------
///
/// This struct holds a single jump in the program, it's needed for reverse step
///
/// e.g. +++>[+.-]+++ This program jumps on step 5 the distance 4
//
typedef struct
{
  unsigned long long step_;      //defines at what step the program was jumping
  unsigned long long distance_;  //the jumps distance, positive or negative
} Jump;

//------------------------------------------------------------------------------
///
/// This struct holds all the jumps in an array
//
typedef struct
{
  unsigned int count_;           //count of the jumps
  size_t allocated_memory_;      //size of the Jump array
  Jump *array_;                  //array where the jumps are stored
} Jumps;


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
  size_t allocated_memory_;      //size of the Jump array
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
  unsigned char **data_segment_; //pointer to the data segment
  size_t *data_length_;          //length of the data segment
  char *program_counter_;        //pointer pointing to the current command
  unsigned char **data_pointer_; //pointer pointing to the current data byte
  long long steps_;              //the maximal steps to run, 0 if infinity
  int *breakpoints_;             //array with the breakpoints
  size_t breakpoint_count_;      //size of the breakpoint array
  unsigned long long step_counter_;   //counts the steps the program makes
  int activate_reverse_step_;
  Jumps jumps_;                  //instance of the Jumps struct
  OverwrittenDataBytes overwrittenDataBytes_;
  char **jump_points_;
  int *size_of_jump_points_;
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
  struct Loop_ *next;
  int cell_;
} Loop;

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
/// @param pointer to the interpreter arguments
/// @param pointer to the command line arguments
//
int runOnce(InterpreterArguments *arguments,
            CommandLineArguments *command_line_arguments);


//------------------------------------------------------------------------------
///
///
//
void setBreakpoint(int program_loaded, InterpreterArguments *arguments,
                   size_t *breakpoint_size);


//------------------------------------------------------------------------------
///
///
//
int compareFunction(const void *a, const void *b);


//------------------------------------------------------------------------------
///
///
//
void step(int program_loaded, InterpreterArguments *arguments, int bonus);


//------------------------------------------------------------------------------
///
///
//
void memory(int program_loaded, unsigned char *data_segment,
            unsigned char *data_pointer_);


//------------------------------------------------------------------------------
///
///
//
void show(int program_loaded, char *program_counter);


//------------------------------------------------------------------------------
///
///
//
void change(int program_loaded, unsigned char *data_segment,
            unsigned char *data_pointer);


//------------------------------------------------------------------------------
///
///
//
void binary(char number, char *binary_number, int digits);


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
int load(char *filedirectory, InterpreterArguments *interpreter_arguments,
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
void eval(InterpreterArguments *arguments, char *input_bfstring, int bonus);


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
int check_code(InterpreterArguments *interpreter_arguments, int character,
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
/// This function frees all allocated memory in the InterpreterArguments struct
///
/// @param pointer to the InterpreterArguments struct
//
void freeInterpreterArguments(InterpreterArguments *interpreterArguments);


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
/// Expands the data segment by twice the size and sets the new memory to 0
///
/// @param data_segment double pointer to the data segment
/// @param data_length current size of the data segment
//
void expandDataSegment(unsigned char **data_segment, size_t *data_length,
                       unsigned char **data_pointer);


void processUserInput(InterpreterArguments *interpreter_arguments,
                        int direction);


//------------------------------------------------------------------------------
///
/// Processes a brainfuck loop
/// If the direction is positive it checks the condition, if it should jump it
/// calls jumpToMatchingBrace
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
unsigned long long jumpToMatchingBrace(InterpreterArguments
                                        *interpreter_arguments);


//------------------------------------------------------------------------------
///
/// This function inserts a jump in the jump array and expands it if necessary
///
/// @param jumps pointer to the Jumps struct
/// @param jump the jump to insert
//
void insertJump(Jumps *jumps, Jump jump);

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
/// TODO
//
void newJumpPoint(InterpreterArguments *interpreter_arguments);


int interpreterBio(InterpreterArguments *interpreter_arguments);


int loadBio(char *file_directory, InterpreterArguments *arguments);


int getNextCmd(char **program_ctr, int *cell);


void push(Loop **top, char *data_ptr, int *cell);


char *pop(Loop **top);







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

  CommandLineArguments command_line_arguments = {0, NULL, 0};
  int eof = 0;

  parseCommandLineArguments(&command_line_arguments, argc, argv);

  if (command_line_arguments.e_ == 1)
  {
    return runOnce(&arguments, &command_line_arguments);
  }

  if (command_line_arguments.r_ == 1)
  {
    arguments.activate_reverse_step_ = 1;
  }

  int program_loaded = NOT_LOADED;
  size_t breakpoint_size = 10;
  // allocate char array for console input
  size_t line_size = 100;
  char *line = calloc(line_size, sizeof(char));


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

    char *cmd = strtok(line, "  ");
    if (cmd)
    {
      if (strcmp(cmd, "load") == 0)
      {
        // get path to file
        cmd = strtok(NULL, " ");
        if (cmd != NULL)
        {
          // get a pointer to the first occurrence of '.'
          char *ext = strrchr(cmd, '.');
          if (command_line_arguments.b_ && ext) //TODO: b_ == 1?
          {
            if(strcmp(ext, ".bio") == 0)
            {
              arguments.loaded_language = 1;
            }
            else // can be .bf or anything else to use bf input language
            {
              arguments.loaded_language = 0;
            }
          }
          else // no file extension or bonus not set, use bf as default
          {
            arguments.loaded_language = 0;
          }
          int return_code = 0;
          if(arguments.loaded_language == 0)
          {
            return_code = load(cmd, &arguments, command_line_arguments.b_);
          }
          else if(arguments.loaded_language == 1)
          {
            return_code = loadBio(cmd, &arguments);
          }
          if (return_code == SUCCESS)
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
          int stop_reason = -1;
          if(arguments.loaded_language == 0)
          {
            stop_reason = interpreter(&arguments);
          }
          else if(arguments.loaded_language == 1)
          {
            stop_reason = interpreterBio(&arguments);
          }
          if (stop_reason == REGULAR_STOP) // ran to the end
          {
            // code reset
            memset(arguments.program_, 0, arguments.program_length_);

            // TODO: what should happen, if run is called twice?
            // breakpoint reset
            free(arguments.breakpoints_);
            arguments.breakpoints_ = NULL;
            breakpoint_size = 10;

            program_loaded = NOT_LOADED;
          }
          else if (stop_reason == STEP_STOP)
          {
            //should never be reached
          }
          else if (stop_reason == BREAKPOINT_STOP) // stopped at breakpoint
          {
            // shift unused breakpoints left to remove already used breakpoint
            memmove(arguments.breakpoints_, arguments.breakpoints_ + 1,
                    (arguments.breakpoint_count_ - 1) * sizeof(int));
            arguments.breakpoint_count_--;
          }
        }
      }
      else if (strcmp(cmd, "eval") == 0)
      {
        char *bfstring = strtok(NULL, " ");
        eval(&evalArguments, bfstring, command_line_arguments.b_);

        //only set the program_loaded variable if no program was loaded yet
        if (program_loaded == NOT_LOADED)
          program_loaded = LOADED_FROM_EVAL;
      }
      else if (strcmp(cmd, "break") == 0)
      {
        setBreakpoint(program_loaded, &arguments, &breakpoint_size);
      }
      else if (strcmp(cmd, "step") == 0)
      {
        step(program_loaded, &arguments, command_line_arguments.b_);
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
    else
    {
      eof = 1;  //if there's no input, it could be EOF
      //if it's not, loop will run again
    }

    // print command line line output
    printf("esp> ");
  }

  freePointer((void **) &line);

  freeInterpreterArguments(&arguments);

  // set this pointers to NULL so that they are not freed a second time
  evalArguments.data_segment_ = NULL;
  evalArguments.data_pointer_ = NULL;
  evalArguments.data_length_ = NULL;
  freeInterpreterArguments(&evalArguments);

  if (!eof)
    printf("Bye.\n");

  return EXIT_SUCCESS;
}

void push(Loop **top, char *data_ptr, int *cell)
{
  // make new stack item and copy data to it:
  Loop *new_item = malloc(sizeof(Loop));
  new_item->data_ptr_ = data_ptr;
  new_item->cell_ = *cell;

  new_item->next = *top;    // next points to previous top
  *top = new_item;          // top now points to new item
}

char *pop(Loop **top)
{
  Loop *old_top = *top;       // remember the old top
  char *data_ptr = old_top->data_ptr_;   // remember the old data
  *top = old_top->next;       // move top down
  free(old_top);              // now we can free the old StackItem
  return data_ptr;            // and return the data we remembered
}

// TODO: only reserve 3 chars in interpreter_arguments->data_segment_ for BIO (as is written in specs of BIO)
int interpreterBio(InterpreterArguments *interpreter_arguments)
{
  int *cell = calloc(1, sizeof(int));
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
        while(found_right != 0)
        {
          interpreter_arguments->program_counter_++;
          if(*(interpreter_arguments->program_counter_) == '}')
          {
            found_right--;
          }
          else if(*(interpreter_arguments->program_counter_) == '{')
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
      if((*(interpreter_arguments->data_segment_))[loop_stack->cell_] != 0)
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
      if (!((ret == 2 && *(*program_ctr + 3) == '{') || (ret == 4 && *(*program_ctr + 3) == ';')))
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

  return LOADED_FROM_FILE;
}

void exitWrongUsage()
{
  printf("[ERR] usage: ./assa [-e brainfuck_filnename]\n");
  exit(WRONG_USAGE_RETURN_CODE);
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

  if(command_line_arguments->r_ == 1 && command_line_arguments->b_ == 0)
  {
    command_line_arguments->r_ = 0;
  }
}


int runOnce(InterpreterArguments *arguments,
            CommandLineArguments *command_line_arguments)
{
  int return_code = load(command_line_arguments->path, arguments,
                         command_line_arguments->b_);

  if (return_code == FILE_READ_ERROR)
  {
    return FILE_READ_ERROR_RETURN_CODE;
  }
  if (return_code == FILE_PARSE_ERROR)
  {
    return FILE_PARS_ERROR_RETURN_CODE;
  }

  interpreter(arguments);

  freeInterpreterArguments(arguments);

  return EXIT_SUCCESS;
}


void setBreakpoint(int program_loaded, InterpreterArguments *arguments,
                   size_t *breakpoint_size)
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

  // TODO We don't have to sort the breakpoints because the interpreter has to check all, because the program can jump over a breakpoint
  // sort breakpoints ascending (eg. 3 - 7 - 10)
  qsort(arguments->breakpoints_, arguments->breakpoint_count_, sizeof(int),
        compareFunction);
  return;
}

int compareFunction(const void *a, const void *b)
{
  return (*(int *) a - *(int *) b);
}

void step(int program_loaded, InterpreterArguments *arguments, int bonus)
{
  if (program_loaded != LOADED_FROM_FILE)
  {
    printf("[ERR] no program loaded\n");
    return;
  }
  char *steps = strtok(NULL, " ");
  arguments->steps_ = strtol(steps, (char **) NULL, 10);

  if (arguments->steps_ == 0 || (arguments->steps_ < 0 && bonus == 0))
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
  int size = (size_input != NULL) ?
             (int) strtol(size_input, (char **) NULL, 10) : 10;

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

int load(char *file_directory, InterpreterArguments *arguments, int bonus)
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
    bracket_counter += check_code(arguments, character, &position, bonus);

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

  return LOADED_FROM_FILE;
}

void eval(InterpreterArguments *arguments, char *input_bfstring, int bonus)
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
      check_code(arguments, input_bfstring[loopCounter], &position, bonus);
  }
  arguments->program_[position] = '\0';  //end of string

  //if #opened brackets is not #closed brackets
  if (bracket_counter != 0)
    printf("[ERR] parsing of input failed\n");
  else
  {
    interpreter(arguments);
    freePointer((void **) &arguments->program_);
  }
}

int check_code(InterpreterArguments *arguments, int character, int *position,
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

int interpreter(InterpreterArguments *interpreter_arguments)
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

  int break_loop = 0;

  //create a local variable for steps because it should not be changed
  //if -1 runs infinitely long (to the end of the program)
  long long steps = (interpreter_arguments->steps_ == 0) ?
              -1 : interpreter_arguments->steps_;

  //the program must run shifted one to the left, increment at the end
  if (direction == -1)
    interpreter_arguments->program_counter_--;

  for (; *(interpreter_arguments->program_counter_) != 0 && steps != 0;
         steps -= direction)
  {
    //check if a breakpoint is reached
    int *breakpoint;
    for (breakpoint = interpreter_arguments->breakpoints_;
         breakpoint < interpreter_arguments->breakpoints_ +
                        interpreter_arguments->breakpoint_count_; breakpoint++)
    {
      if (interpreter_arguments->program_ + *breakpoint ==
          interpreter_arguments->program_counter_)
      {
        break_loop = 1;
      }
    }
    if (break_loop)
      break;

    // ------------  >  ------------
    if (*(interpreter_arguments->program_counter_) == move_forward)
    {
      if (++(*interpreter_arguments->data_pointer_) >
          *interpreter_arguments->data_segment_ +
          *(interpreter_arguments->data_length_) - 1)
      {
        expandDataSegment(interpreter_arguments->data_segment_,
                          interpreter_arguments->data_length_,
                          interpreter_arguments->data_pointer_);
      }
      interpreter_arguments->program_counter_ += direction;
    }


      // ------------  <  ------------
    else if (*(interpreter_arguments->program_counter_) == move_back)
    {
      if (--(*interpreter_arguments->data_pointer_) <
          *interpreter_arguments->data_segment_)
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
      newJumpPoint(interpreter_arguments);
      //**interpreter_arguments->data_pointer_ = 0;

      interpreter_arguments->program_counter_ += direction;
    }


      // ------------  %  ------------
    else if (*(interpreter_arguments->program_counter_) == jump)
    {
      interpreter_arguments->program_counter_ =
        interpreter_arguments->jump_points_[
          **interpreter_arguments->data_pointer_];

      interpreter_arguments->program_counter_ += direction;
    }

    interpreter_arguments->step_counter_ += direction;
  }

  if (direction == -1)
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

  memset(*interpreter_arguments->data_segment_, 0,
         *interpreter_arguments->data_length_ * sizeof(unsigned char));

  *interpreter_arguments->data_pointer_ = *interpreter_arguments->data_segment_;

  interpreter_arguments->breakpoint_count_ = 0;
}

InterpreterArguments getUsableInterpreterArgumentsStruct(
  unsigned char **data_segment, size_t *data_length,
  unsigned char **data_pointer)
{
  InterpreterArguments interpreter_arguments = {
    NULL,         // program_
    0,            // program_length_
    data_segment, // data_segment_
    data_length,  // data_length_
    NULL,         // program_counter_
    data_pointer, // data_pointer_
    0,            // steps_
    NULL,         // breakpoints_
    0,            // breakpoint_count_
    0,            // step_counter_
    0,            // activate_reverse_step_
    {             // jumps_
      0,            // count_
      0,            // allocated_memory_
      NULL          // array_
    },
    {             // inputs
      0,            // count_
      0,            // allocated_memory_
      NULL          // array_
    },
    NULL,         // jump_points_
    NULL,         // size_of_jump_points_
    -1            // loaded_language
  };


  if (data_segment == NULL)
  {
    interpreter_arguments.data_length_ = saveMalloc(sizeof(size_t));
    *interpreter_arguments.data_length_ = 1024;

    interpreter_arguments.data_segment_ = saveMalloc(sizeof(unsigned char *));
    *interpreter_arguments.data_segment_ =
      calloc(*interpreter_arguments.data_length_, sizeof(unsigned char));

    interpreter_arguments.data_pointer_ = saveMalloc(sizeof(unsigned char *));
    *interpreter_arguments.data_pointer_ = *interpreter_arguments.data_segment_;
  }

  interpreter_arguments.jumps_.allocated_memory_ = 200;
  interpreter_arguments.jumps_.array_ = (Jump *) saveMalloc(
    interpreter_arguments.jumps_.allocated_memory_ * sizeof(Jump));

  interpreter_arguments.overwrittenDataBytes_.allocated_memory_ = 20;
  interpreter_arguments.overwrittenDataBytes_.array_ = (OverwrittenDataByte *) saveMalloc(
    interpreter_arguments.overwrittenDataBytes_.allocated_memory_ * sizeof(OverwrittenDataByte));

  //*interpreter_arguments.jump_points_ = saveMalloc(1);

  return interpreter_arguments;
}

void freeInterpreterArguments(InterpreterArguments *interpreterArguments)
{
  freePointer((void **) &(interpreterArguments->program_));
  freeDoublePointer((void ***) &(interpreterArguments->data_segment_));
  freePointer((void **) &(interpreterArguments->data_length_));
  freePointer((void **) &(interpreterArguments->data_pointer_));
  freePointer((void **) &(interpreterArguments->breakpoints_));
  freePointer((void **) &(interpreterArguments->jumps_.array_));
  freePointer((void **) &(interpreterArguments->overwrittenDataBytes_.array_));
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

void expandDataSegment(unsigned char **data_segment, size_t *data_length,
                       unsigned char **data_pointer)
{
  unsigned long data_pointer_offset = *data_pointer - *data_segment;

  *data_length *= 2;
  *data_segment = saveRealloc(*data_segment, *data_length);

  *data_pointer = *data_segment + data_pointer_offset;

  //set the new memory to 0
  memset(*data_segment + (*data_length) / 2, 0, (*data_length) / 2);

  printf("Expand %lu\n", *data_length);
}

void processUserInput(InterpreterArguments *interpreter_arguments,
                      int direction)
{
  if (direction == 1)
  {
    insertOverwrittenDataByte(&(interpreter_arguments->overwrittenDataBytes_),
                              (OverwrittenDataByte) {
                                interpreter_arguments->step_counter_,
                                **interpreter_arguments->data_pointer_
                              });

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
           interpreter_arguments->overwrittenDataBytes_.count_-1;
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
      unsigned long long distance = jumpToMatchingBrace(interpreter_arguments);

      if(interpreter_arguments->activate_reverse_step_ == 1)
      {
        insertJump(&interpreter_arguments->jumps_,
                   (Jump) {interpreter_arguments->step_counter_, distance});
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
    for (search_loop_index = interpreter_arguments->jumps_.count_;
         search_loop_index > 0; search_loop_index--)
    {
      //if the program was jumping
      if (interpreter_arguments->jumps_.array_[search_loop_index].step_ ==
          interpreter_arguments->step_counter_ - 1)
      {
        //jump in the opposite direction
        interpreter_arguments->program_counter_ -=
          interpreter_arguments->jumps_.array_[search_loop_index].distance_;

        jump_found = 1;
      }
    }

    if (jump_found == 0)
      interpreter_arguments->program_counter_--;
  }
}

unsigned long long jumpToMatchingBrace(InterpreterArguments
                                        *interpreter_arguments)
{
  int loop_counter = 1;
  unsigned long long distance = 0;
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

  if (direction == -1)
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

  if (jumps->count_ == jumps->allocated_memory_)
  {
    //extend array
    jumps->allocated_memory_ *= 2;
    jumps->array_ = saveRealloc(jumps->array_,
                                jumps->allocated_memory_ * sizeof(Jump));
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

void newJumpPoint(InterpreterArguments *interpreter_arguments)
{
  if (**interpreter_arguments->data_pointer_ >
      *interpreter_arguments->size_of_jump_points_)
  {
    interpreter_arguments->jump_points_ =
      saveRealloc(interpreter_arguments->jump_points_,
                  sizeof(char) * **interpreter_arguments->data_pointer_);

    *interpreter_arguments->size_of_jump_points_ =
      **interpreter_arguments->data_pointer_;
  }

  interpreter_arguments->jump_points_[**interpreter_arguments->data_pointer_] =
    interpreter_arguments->program_counter_;
}