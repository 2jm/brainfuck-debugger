#include "interpreter.h"

//cool for debugging the interpreter, you won't need this
void dumpDataSegment(unsigned char* data, size_t length);

int main()
{
  int return_value;

  /*
   *  THE CODE
   */
  // The code comes from a file or the stdin
  char *code = "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.";
  char *evalCode = "<+>+>+<";
  //char *code = ">+++[>+++[<<+.>>-]<-]>[++]++";

  /*
   *  CREATE THE ARGUMENTS FOR AN NORMAL CALL
   */
  // Get an InterpreterArguments struct
  InterpreterArguments arguments =
    getUsableInterpreterArgumentsStruct(NULL, NULL, NULL);

  // This is usually done in the load function:
  arguments.program_ = malloc(500 * sizeof(char));  //allocate memory for the code
  strcpy(arguments.program_, code);                 //copy code into struct
  arguments.program_counter_ = arguments.program_;  //set program_counter_ to the beginning of the code

  /*
   *  CREATE THE ARGUMENTS FOR AN EVAL CALL
   */
  InterpreterArguments evalArguments =
    getUsableInterpreterArgumentsStruct(arguments.data_segment_,
                                        arguments.data_length_,
                                        arguments.data_pointer_);
  // save the eval code in the struct
  evalArguments.program_ = malloc(500 * sizeof(char));  //allocate memory for the code
  strcpy(evalArguments.program_, evalCode);                 //copy code into struct
  evalArguments.program_counter_ = evalArguments.program_;  //set program_counter_ to the beginning of the code


  /*
   *  CREATE A BREAKPOINT ON POSITION 60
   */
  arguments.breakpoints_ = malloc(10 * sizeof(int));
  arguments.breakpoints_[0] = 60;
  arguments.breakpoint_count_++;


  /*
   *  RUN NORMAL
   */
  // run the interpreter with the normal arguments it will break on 60
  return_value = interpreter(&arguments);

  // print the return value it will be 2
  printf("\nSTOP: %d\n", return_value);
  // remove the breakpoint (oderso)
  arguments.breakpoint_count_--;


  /*
   *  RUN THE INTERPRETER WITH THE EVAL CODE
   */
  // this code will increase the 3 data bytes by 1, this will change the output
  return_value = interpreter(&evalArguments);
  // print the return value it will be 0
  printf("\nSTOP: %d\n", return_value);


  /*
   *  RUN NORMAL AGAIN TO THE END
   */
  return_value = interpreter(&arguments);
  // print the return value it will be 0
  printf("\nSTOP: %d\n", return_value);

  // print the first 10 bytes of the data segment
  dumpDataSegment(*arguments.data_segment_, 10);

  return 0;
}



void dumpDataSegment(unsigned char* data, size_t length)
{
  size_t i;
  for(i=0; i<length; i++)
  {
    printf("%d ", data[i]);
  }
  printf("\n");
}