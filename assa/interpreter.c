//
// Created by jonas on 24.11.15.
//

#include "interpreter.h"


void expandDataSegment (unsigned char **data_segment, size_t *data_length);

int interpreter (InterpreterArguments *interpreter_arguments)
{
  int direction = (interpreter_arguments->steps_ < 0) ? -1 : 1;
  int break_loop = 0;

  //if -1 runs infinitely long (to the end of the program)
  int steps = (interpreter_arguments->steps_ == 0) ? -1 : interpreter_arguments->steps_;

  for (; *(interpreter_arguments->program_counter_) != 0 && steps != 0;
         steps -= direction)
  {
    //check if a breakpoint is reached
    int *breakpoint = interpreter_arguments->breakpoints_;
    for(; breakpoint < interpreter_arguments->breakpoints_ + interpreter_arguments->breakpoint_count_; breakpoint++)
    {
      if(interpreter_arguments->program_ + *breakpoint == interpreter_arguments->program_counter_)
      {
        break_loop = 1;
      }
    }
    if(break_loop)
      break;

    //interpret command
    switch(*(interpreter_arguments->program_counter_))
    {
      case '>':
        if (++(interpreter_arguments->data_pointer_) > interpreter_arguments->data_segment_ + interpreter_arguments->data_length_)
        {
          expandDataSegment (&interpreter_arguments->data_segment_, &interpreter_arguments->data_length_);
        }
            
        interpreter_arguments->program_counter_ += direction;
        break;

      case '<':
        if (--(interpreter_arguments->data_pointer_) < interpreter_arguments->data_segment_)
        {
          (interpreter_arguments->data_pointer_)++;
        }

        interpreter_arguments->program_counter_ += direction;
        break;

      case '+':
        (*interpreter_arguments->data_pointer_)++;

        interpreter_arguments->program_counter_ += direction;
        break;

      case '-':
        (*interpreter_arguments->data_pointer_)--;

        interpreter_arguments->program_counter_ += direction;
        break;

      case '.':
        putchar (*interpreter_arguments->data_pointer_);

        interpreter_arguments->program_counter_ += direction;
        break;

      case ',':
        *interpreter_arguments->data_pointer_ = (unsigned char) getchar ();

        interpreter_arguments->program_counter_ += direction;
        break;

      case '[':
        if (*interpreter_arguments->data_pointer_ == 0)
        {
          int loop_counter = 1;

          interpreter_arguments->program_counter_++;

          while (loop_counter > 0 && interpreter_arguments->program_counter_ != 0)
          {
            if (*(interpreter_arguments->program_counter_) == '[')
            {
              loop_counter++;
            }
            else if (*(interpreter_arguments->program_counter_) == ']')
            {
              loop_counter--;
            }

            interpreter_arguments->program_counter_++;
          }

          // The program_counter was increment one too much
          interpreter_arguments->program_counter_--;
        }
        else
        {
          interpreter_arguments->program_counter_++;
        }
        break;

      case ']':
        if (*interpreter_arguments->data_pointer_ == 0)
        {
          interpreter_arguments->program_counter_++;
        }
        else
        {
          int loop_counter = 1;

          interpreter_arguments->program_counter_--;

          while (loop_counter > 0 && interpreter_arguments->program_counter_ > interpreter_arguments->program_)
          {
            if (*(interpreter_arguments->program_counter_) == ']')
            {
              loop_counter++;
            }
            else if (*(interpreter_arguments->program_counter_) == '[')
            {
              loop_counter--;
            }

            interpreter_arguments->program_counter_--;
          }

          // The program_counter was increment one too much
          interpreter_arguments->program_counter_++;
        }
        break;

      //should never be reached
      default:
        break;
    }
  }

  if (steps == 0) //program ran the specified steps to the end
  {
    return -1;
  }

  if (break_loop)
  {
    return -2;
  }

  return 0;
}

void expandDataSegment (unsigned char **data_segment, size_t *data_length)
{
  *data_length *= 2;
  *data_segment = realloc (*data_segment, *data_length);

  //set the new memory to 0
  memset (*data_segment + (*data_length) / 2, 0, (*data_length) / 2);
}




/*if((interpreter_arguments->program_counter_ == '>' && direction == 1) ||
          (interpreter_arguments->program_counter_ == '<' && direction == -1))
    {
      if(++(interpreter_arguments->data_pointer_) > *data_segment + *data_length)
        expandDataSegment(data_segment, data_length);
    }
    else if((interpreter_arguments->program_counter_ == '<' && direction == 1) ||
        (interpreter_arguments->program_counter_ == '>' && direction == -1))
    {
      if(--(interpreter_arguments->data_pointer_) < *data_segment)
        (interpreter_arguments->data_pointer_)++;
    }
    else if((interpreter_arguments->program_counter_ == '+' && direction == 1) ||
          (interpreter_arguments->program_counter_ == '-' && direction == -1))
    {
      (*interpreter_arguments->data_pointer_)++;
    }
    else if((interpreter_arguments->program_counter_ == '-' && direction == 1) ||
          (interpreter_arguments->program_counter_ == '+' && direction == -1))
    {
      (*interpreter_arguments->data_pointer_)--;
    }
    else if(interpreter_arguments->program_counter_ == '.')
    {

    }
    else if(interpreter_arguments->program_counter_ == ',' && direction == 1)
    {

    }
    else if((interpreter_arguments->program_counter_ == '[' && direction == 1) ||
          (interpreter_arguments->program_counter_ == ']' && direction == -1))
    {
      if(*interpreter_arguments->data_pointer_ == 0)
      {
        int loop_counter = 1;

        while(loop_counter > 0 && interpreter_arguments->program_counter_ != 0)
        {
          if(interpreter_arguments->program_counter_ == '[')
          {
            loop_counter++;
          }
          else if(interpreter_arguments->program_counter_ == ']')
          {
            loop_counter--;
          }

          interpreter_arguments->program_counter_++;
        }

        // The program_counter was increment one to much
        interpreter_arguments->program_counter_--;
      }
      else
      {
        interpreter_arguments->program_counter_++;
      }
    }
    else if((interpreter_arguments->program_counter_ == ']' && direction == 1) ||
          (interpreter_arguments->program_counter_ == '[' && direction == -1))
    {
      if(*interpreter_arguments->data_pointer_ == 0)
      {
        interpreter_arguments->program_counter_++;
      }
      else
      {
        int loop_counter = 1;

        while(loop_counter > 0 && *program_counter > program)
        {
          if(interpreter_arguments->program_counter_ == ']')
          {
            loop_counter++;
          }
          else if(interpreter_arguments->program_counter_ == '[')
          {
            loop_counter--;
          }

          interpreter_arguments->program_counter_--;
        }

        // The program_counter was increment one to much
        interpreter_arguments->program_counter_++;
      }
    }*/