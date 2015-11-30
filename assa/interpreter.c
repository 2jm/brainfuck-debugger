//
// Created by jonas on 24.11.15.
//

#include "interpreter.h"


void expandDataSegment (unsigned char **data_segment, size_t *data_length);

int interpreter (InterpreterArguments *interpreter_arguments)
{
  int direction = (interpreter_arguments->steps_ < 0) ? -1 : 1;

  if (interpreter_arguments->steps_ == 0)
  {
    interpreter_arguments->steps_ = -1;
  }   //runs infinitely long (to the end of the program)

  for (; interpreter_arguments->program_counter_ != 0 && interpreter_arguments->steps_ != 0 
    /*&& interpreter_arguments->program_counter != program+breakpoint*/; 
         interpreter_arguments->steps_ -= direction)
  {
    //printf("%c %d - ", interpreter_arguments->program_counter_, steps);
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

      //ignore default
      default:
        break;
    }
  }

  if (interpreter_arguments->steps_ == 0) //program ran the specified steps to the end
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