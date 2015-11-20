//-----------------------------------------------------------------------------
// assa.c
//
// Strings and memory management
//
// Group: 13020 study assistant Angela Promitzer
//
// Authors: Johannes Kopf 1431505, Jonas Juffinger, Matthias Klotz
//
// Latest Changes: 20.11.2015 (by Johannes Kopf)
//-----------------------------------------------------------------------------
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-----------------------------------------------------------------------------
///
/// Reads a line from the console till the EOF is given as input.
///
/// @param bufferLen The buffer length that is set inside the function to get
/// access to it outside.
//
char *readLine(int *bufferLen)
{
  int ch;
  int len;
  char *buffer = malloc(sizeof(char));
  if(buffer == NULL)
  {
    return NULL;
  }
  for (len = 0; (ch = getchar()) != '\n'; len++) //EOF
  {
    buffer[len] = (char) ch;
    char *new_mem = realloc(buffer, (len + 2) * sizeof(char)); // +1 due to
    // index, +1 due to \0
    if(new_mem != NULL)
    {
      buffer = new_mem;
    }
    else
    {
      free(buffer);
      return NULL;
    }
  }
  buffer[len] = '\0'; // add string null-termination character
  *bufferLen = len;
  return buffer;
}

char *simplifyWhitespaces(char *segments)
{
  int len = strlen(segments);
  int cnt;
  for (cnt = 0; cnt < len; cnt++)
  {
    if(segments[cnt] == ' ' && segments [cnt+1]==' ')
    {
      int remove_from;
      // remove the element at the position cnt
      for (remove_from = cnt; remove_from < len - 1 ; remove_from++)
      {
        segments[remove_from] = segments[remove_from + 1];
      }
    }
  }
  return segments;
}

char *getCommands(char *segments)
{
  return segments;
}

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
  //int segment_size = 1024; // 1024 Bytes (0 - 1023)
  //unsigned char *segment = malloc(segment_size * sizeof(unsigned char));

  int interactive_mode = 1;

  char quit[] = "quit";

  if(interactive_mode)
  {
    char *line = NULL;
    do
    {
      if(line != NULL)
      {
        printf("Debug: Line free\n");
        // free all allocated memory
        free(line);
      }

      printf("esp>");

      // read from the console/other input
      int line_len = 0;
      line = readLine(&line_len);

      printf("Read: %s\n", line);

      line = simplifyWhitespaces(line);

      printf("Trimmed: %s\n", line);
    }
    while(strcmp(line, quit));
  }

  printf("Bye.\n");

//  int line_len = 0;
//  char *line = readLine(&line_len);
//  if(line == NULL)
//  {
//    printf("Error: Out of memory!\n");
//    return 1;
//  }
//  // print the read value to the console in reversed order
//  int print_cnt;
//  for (print_cnt = line_len - 1; print_cnt >= 0; print_cnt--)
//  {
//    printf("%c", line[print_cnt]);
//  }
//  printf("\n");
  return 0;
}
