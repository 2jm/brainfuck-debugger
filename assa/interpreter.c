//
// Created by jonas on 24.11.15.
//

#include "interpreter.h"


void expandDataSegment(char **data_segment, size_t *data_length);

int interpreter(char *program, char **data_segment, size_t *data_length,
                 char **program_counter, char **data_pointer, int steps)
{
  int direction = (steps < 0) ? -1 : 1;

  for(; **program_counter != 0 && steps != 0; steps -= direction)
  {
    if((**program_counter == '>' && direction == 1) ||
          (**program_counter == '<' && direction == -1))
    {
      if(++(*data_pointer) > *data_segment + *data_length)
        expandDataSegment(data_segment, data_length);
    }
    else if((**program_counter == '<' && direction == 1) ||
        (**program_counter == '>' && direction == -1))
    {
      if(--(*data_pointer) < *data_segment)
        (*data_pointer)++;
    }
    else if((**program_counter == '+' && direction == 1) ||
          (**program_counter == '-' && direction == -1))
    {
      (**data_pointer)++;
    }
    else if((**program_counter == '-' && direction == 1) ||
          (**program_counter == '+' && direction == -1))
    {
      (**data_pointer)--;
    }
    else if(**program_counter == '.')
    {

    }
    else if(**program_counter == ',' && direction == 1)
    {

    }
    else if((**program_counter == '[' && direction == 1) ||
          (**program_counter == ']' && direction == -1))
    {
      if(**data_pointer == 0)
      {
        int loop_counter = 1;

        while(loop_counter > 0 && **program_counter != 0)
        {
          if(**program_counter == '[')
          {
            loop_counter++;
          }
          else if(**program_counter == ']')
          {
            loop_counter--;
          }

          (*program_counter)++;
        }

        // The program_counter was increment one to much
        (*program_counter)--;
      }
      else
      {
        (*program_counter)++;
      }
    }
    else if((**program_counter == ']' && direction == 1) ||
          (**program_counter == '[' && direction == -1))
    {
      if(**data_pointer == 0)
      {
        (*program_counter)++;
      }
      else
      {
        int loop_counter = 1;

        while(loop_counter > 0 && *program_counter > program)
        {
          if(**program_counter == ']')
          {
            loop_counter++;
          }
          else if(**program_counter == '[')
          {
            loop_counter--;
          }

          (*program_counter)--;
        }

        // The program_counter was increment one to much
        (*program_counter)++;
      }
    }


    /*switch(**program_counter)
    {
      case '>':
        if(++(*data_pointer) > *data_segment + *data_length)
          expandDataSegment(data_segment, data_length);
        break;

      case '<':
        if(--(*data_pointer) < *data_segment)
          (*data_pointer)++;
        break;

      case '+':
        (**data_pointer)++;
        break;

      case '-':
        (**data_pointer)--;
        break;

      case '.':
        putchar(**data_pointer);
        break;

      case ',':
        **data_pointer = (char) getchar();
        break;

      case '[':
        if(**data_pointer == 0)
        {
          int loop_counter = 1;

          while(loop_counter > 0 && **program_counter != 0)
          {
            if(**program_counter == '[')
            {
              loop_counter++;
            }
            else if(**program_counter == ']')
            {
              loop_counter--;
            }

            (*program_counter)++;
          }

          // The program_counter was increment one to much
          (*program_counter)--;
        }
        else
        {
          (*program_counter)++;
        }
        break;

      case ']':
        if(**data_pointer == 0)
        {
          (*program_counter)++;
        }
        else
        {
          int loop_counter = 1;

          while(loop_counter > 0 && *program_counter > program)
          {
            if(**program_counter == ']')
            {
              loop_counter++;
            }
            else if(**program_counter == '[')
            {
              loop_counter--;
            }

            (*program_counter)--;
          }

          // The program_counter was increment one to much
          (*program_counter)++;
        }
        break;
    }*/
  }

  return 1;
}


void expandDataSegment(char **data_segment, size_t *data_length)
{
  *data_length *= 2;
  *data_segment = realloc(*data_segment, *data_length);
}