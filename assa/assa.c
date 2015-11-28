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
#include "debugFunctions.h"
#include "interpreter.h"

////-----------------------------------------------------------------------------
/////
///// Reads a line from the console till the EOF is given as input.
/////
///// @param bufferLen The buffer length that is set inside the function to get
///// access to it outside.
////
//char *readLine (int *bufferLen)
//{
//  int ch;
//  int len;
//  char *buffer = malloc (sizeof (char));
//  if (buffer == NULL)
//  {
//    return NULL;
//  }
//  for (len = 0; (ch = getchar ()) != '\n'; len++) //EOF
//  {
//    buffer[len] = (char) ch;
//    char *new_mem = realloc (buffer, (len + 2) * sizeof (char)); // +1 due to
//    // index, +1 due to \0
//    if (new_mem != NULL)
//    {
//      buffer = new_mem;
//    }
//    else
//    {
//      free (buffer);
//      printf ("[ERR] out of memory\\n");
//      return NULL;
//    }
//  }
//  buffer[len] = '\0'; // add string null-termination character
//  *bufferLen = len;
//  return buffer;
//}

//char *simplifyWhitespaces (char *segments)
//{
//  int len = strlen (segments);
//  int cnt;
//  for (cnt = 0; cnt < len; cnt++)
//  {
//    if (segments[cnt] == ' ' && segments[cnt + 1] == ' ')
//    {
//      int remove_from;
//      // remove the element at the position cnt
//      for (remove_from = cnt; remove_from < len - 1; remove_from++)
//      {
//        segments[remove_from] = segments[remove_from + 1];
//      }
//    }
//  }
//  return segments;
//}

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
int main (int argc, char *argv[])
{
  //TODO: unsigned char *data_segment =calloc (data_segment_size, sizeof (unsigned char));

  size_t data_segment_size = 1024; // 1024 Bytes (0 - 1023)
  // init datasegment with 0s
  char *data_segment = calloc (data_segment_size, sizeof (char));
  // init datasegment with 0s - variant 2
  //char *data_segment = malloc (data_segment_size * sizeof (char));
  //memset(data_segment, 0, data_segment_size * sizeof(char));
  char *data_pointer = data_segment;

  size_t code_size = 1024;
  char *code = calloc (code_size, sizeof (char));
  char *code_position = code;

  int breakpoint_count = 0;
  int *breakpoints = NULL;

  int line_size = 100;

  char *line = malloc (sizeof (char) * line_size);
  while (strcmp (line, "quit")) // != 0
  {
    printf ("esp>");
    // read from the console till '\n'
    fgets (line, line_size, stdin);
    // fgets liest am Ende \n und dann \0 ein. Überschreibe also das \n.
    line[strlen (line) - 1] = '\0';

    char *cmd;
    cmd = strtok (line, "  ");
    //TODO: -e soll laut Spezifikation als Argument übergeben werden, nicht
    //TODO: in der Command line!
    if (strcmp (cmd, "-e") == 0)
    {
      cmd = strtok (NULL, " ");
      load (cmd, code);
      run (code, &data_segment, &data_segment_size, &code_position,
           &data_pointer, breakpoints);

      // code reset
      memset (code, 0, code_size * sizeof (char));
    }
    else if (strcmp (cmd, "load") == 0)
    {
      // data reset
      memset (data_segment, 0, data_segment_size * sizeof (char));

      cmd = strtok (NULL, " ");
      load (cmd, code);
    }
    else if (strcmp (cmd, "run") == 0)
    {
      if (strlen (code) == 0)
      {
        printf ("[ERR] no program loaded\n");
        continue;
      }
      if (run (code, &data_segment, &data_segment_size, &code_position,
               &data_pointer, breakpoints) == 0) // ran to the end
      {
        // code reset
        memset (code, 0, code_size * sizeof (char));

        // breakpoint reset
        memset (breakpoints, 0, breakpoint_count * sizeof (int));
        breakpoints = NULL;
      }
      else // stopped at breakpoint
      {
        //TODO: remove breakpoint from breakpoint list
      }
    }
    else if (strcmp (cmd, "eval") == 0)
    {
      char *bfstring = strtok (NULL, " ");
      eval (&data_segment, &data_segment_size, &data_pointer, bfstring);
    }
    else if (strcmp (cmd, "break") == 0)
    {
      if (strlen (code) == 0)
      {
        printf ("[ERR] no program loaded\n");
        continue;
      }
      cmd = strtok (NULL, " ");
      int number = strtol (cmd, (char **) NULL, 10);

      int *new_mem = realloc (breakpoints, ++breakpoint_count * sizeof (int));
      if (new_mem != NULL)
      {
        breakpoints = new_mem;
      }
      else
      {
        free (breakpoints);
        printf ("[ERR] out of memory\\n");
      }
      breakpoints[breakpoint_count - 1] = number;

      //TODO: sort breakpoints here ascending
    }
    else if (strcmp (cmd, "step") == 0)
    {
      if (strlen (code) == 0)
      {
        printf ("[ERR] no program loaded\n");
        continue;
      }
      cmd = strtok (NULL, " ");
      int steps = (int) strtol(cmd, (char**)NULL, 10);

      interpreter(code, &data_segment, &data_segment_size, &code_position,
                  &data_pointer, steps);
//      run (code, &data_segment, &data_segment_size, &code_position,
//           &data_pointer, breakpoints); //== 0
    }
    else if (strcmp (cmd, "memory") == 0)
    {
      if (strlen (code) == 0)
      {
        printf ("[ERR] no program loaded\n");
        continue;
      }
      cmd = strtok (NULL, " ");
      if (cmd == NULL)
      {
        printf ("Hex at %d: %x\n", 0, *(data_segment));
        continue;
      }
      int number = (int) strtol (cmd, (char **) NULL, 10); // 10 = base of digit
      char *type = strtok (NULL, " ");

      //TODO: how to handle if type is not given?

      if (strcmp (type, "hex") == 0)
      {
        printf ("Hex at %d: %x\n", number, *(data_segment + number));
      }
      else if (strcmp (type, "int") == 0)
      {
        printf ("Integer at %d: %d\n", number, *(data_segment + number));
      }
      else if (strcmp (type, "bin") == 0)
      {
        //Source: http://stackoverflow
        // .com/questions/6373093/how-to-print-binary-number-via-printf
        char buffer[33];
        itoa (*(data_segment + number), buffer, 2);
        int binary = strtol(buffer, (char**) NULL, 10);
        printf ("Binary at %d: %08d\n", number, binary);
        //TODO: try if this is possible with strtol
      }
      else if (strcmp (type, "char") == 0)
      {
        printf ("Character at %d: %c\n", number, *(data_segment + number));
      }
    }
    else if (strcmp (cmd, "show") == 0)
    {
      if (strlen (code) == 0)
      {
        printf ("[ERR] no program loaded\n");
        continue;
      }
      cmd = strtok (NULL, " ");
      // 10 is default size
      int size = cmd != NULL ? strtol (cmd, (char **) NULL, 10) : 10;

      // Source: http://stackoverflow
      // .com/questions/4214314/get-a-substring-of-a-char
      // print "size" characters from target string
      // at code position + offset
      printf ("%.*s\n", size, code + (code_position - code));
    }
    else if (strcmp (cmd, "change") == 0)
    {
      if (strlen (code) == 0)
      {
        printf ("[ERR] no program loaded\n");
        continue;
      }
      cmd = strtok (NULL, " ");
      char *end_ptr;
      int number = strtol (cmd, &end_ptr, 10);
      if (end_ptr == cmd) // conversion failed
      {
        // default number: current position
        number = 0;
      }

      cmd = strtok (NULL, " ");
      int hex_byte = (int) strtol (cmd, &end_ptr, 16);
      if (end_ptr == cmd) // conversion failed
      {
        // default hex_byte: 0x0
        hex_byte = 0;
      }

      //TODO: Debug
      printf ("Read: hex: 0x%x - int: %i\n", hex_byte, hex_byte);

      //Specification: Die Hex Eingabe soll nicht mit 0x beginnen!
      // otherwise:
      // int hex_byte = (int)strtol(cmd, NULL, 0);
      //http://stackoverflow.com/questions/10156409/convert-hex-string-char-to-int

      //TODO: funktioniert nicht richtig (change 9f wird zu ffffff9f anstatt ff, 4f
      // stimmt noch - gibt aus: 4f
      *(data_segment + number) = hex_byte;
    }
  }
  free (line);

  // free all variables used above here
  free (data_segment);
  free (code);
  free (breakpoints);

  printf ("Bye.\n");

  return 0;
}
