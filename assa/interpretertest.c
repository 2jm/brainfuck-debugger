#include "interpreter.h"


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
  InterpreterArguments arguments = {
    //"++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.",
    ">+++[>+++[<<+.>>-]<-]>[++]++",
    NULL,
    1024,
    NULL,
    NULL,
    0,
    NULL,
    0,
    0,
    {0, 0, NULL}
  };

  arguments.data_segment_ = calloc(arguments.data_length_, sizeof(char));
  arguments.program_counter_ = arguments.program_;
  arguments.data_pointer_ = arguments.data_segment_;

  arguments.steps_ = 0;

  interpreter(&arguments);

  printf("\n");

  dumpDataSegment(arguments.data_segment_, 3);

  return 0;
}