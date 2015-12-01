#include "interpreter.h"

//cool for debugging the interpreter, you don't need this
void dumpDataSegment(unsigned char* data, size_t length)
{
  size_t i;
  for(i=0; i<length; i++)
  {
    printf("%d ", data[i]);
  }
  printf("\n");
}

int main()
{
  // The code comes from a file or the stdin
  //char *code = "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.",
  char *code = ">+++[>+++[<<+.>>-]<-]>[++]++";

  // Get an InterpreterArguments struct
  InterpreterArguments arguments = getUseableInterpreterArgumentsStruct();

  // This is usually done in the load or eval function:
  arguments.program_ = malloc(500 * sizeof(char));  //allocate memory for the code
  strcpy(arguments.program_, code);                 //copy code into struct
  arguments.program_counter_ = arguments.program_;  //set program_counter_ to the beginning of the code

  // Run the interpreter
  interpreter(&arguments);

  return 0;
}