//
// Created by jonas on 24.11.15.
//

#include "interpreter.h"

void expandDataSegment (char **data_segment, size_t *data_length);

int interpreter (char *program, char **data_segment, size_t *data_length,
                 char **program_counter, char **data_pointer, int steps,
                 int breakpoint)
{
  int direction = (steps < 0) ? -1 : 1;

  if (steps == 0)
  {
    steps = -1;
  }   //runs infinitely long (to the end of the program)

  for (; **program_counter != 0 && steps != 0 && *program_counter != program+breakpoint; steps -= direction)
  {
    //printf("%c %d - ", **program_counter, steps);
    switch (**program_counter)
    {
      case '>':
        if (++(*data_pointer) > *data_segment + *data_length)
        {
          expandDataSegment (data_segment, data_length);
        }

        *program_counter += direction;
        break;

      case '<':
        if (--(*data_pointer) < *data_segment)
        {
          (*data_pointer)++;
        }

        *program_counter += direction;
        break;

      case '+':
        (**data_pointer)++;

        *program_counter += direction;
        break;

      case '-':
        (**data_pointer)--;

        *program_counter += direction;
        break;

      case '.':
        putchar (**data_pointer);

        *program_counter += direction;
        break;

      case ',':
        **data_pointer = (char) getchar ();

        *program_counter += direction;
        break;

      case '[':
        if (**data_pointer == 0)
        {
          int loop_counter = 1;

          (*program_counter)++;

          while (loop_counter > 0 && **program_counter != 0)
          {
            if (**program_counter == '[')
            {
              loop_counter++;
            }
            else if (**program_counter == ']')
            {
              loop_counter--;
            }

            (*program_counter)++;
          }

          // The program_counter was increment one too much
          (*program_counter)--;
        }
        else
        {
          (*program_counter)++;
        }
        break;

      case ']':
        if (**data_pointer == 0)
        {
          (*program_counter)++;
        }
        else
        {
          int loop_counter = 1;

          (*program_counter)--;

          while (loop_counter > 0 && *program_counter > program)
          {
            if (**program_counter == ']')
            {
              loop_counter++;
            }
            else if (**program_counter == '[')
            {
              loop_counter--;
            }

            (*program_counter)--;
          }

          // The program_counter was increment one too much
          (*program_counter)++;
        }
        break;
    }
  }

  if (steps == 0) //program ran the specified steps to the end
  {
    return -1;
  }
//  else if (steps < 0)
//  {
  return 0;
//  }
//  else
//  {
//    return 1; //error
//  }
}

void expandDataSegment (char **data_segment, size_t *data_length)
{
  *data_length *= 2;
  *data_segment = realloc (*data_segment, *data_length);

  //set the new memory to 0
  memset (*data_segment + (*data_length) / 2, 0, (*data_length) / 2);
}




/*if((**program_counter == '>' && direction == 1) ||
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
    }*/