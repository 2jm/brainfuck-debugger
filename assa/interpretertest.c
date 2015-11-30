#include "interpreter.h"

int main()
{
  char *code = "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.";
  size_t data_segment_size = 1024;
  char *data_segment = calloc(data_segment_size, sizeof(char));

  char *program_counter = code;
  char *data_pointer = data_segment;

  printf("Run interpreter:\n");

  interpreter(code, &data_segment, &data_segment_size, &program_counter,
              &data_pointer, 0);

  return 0;
}