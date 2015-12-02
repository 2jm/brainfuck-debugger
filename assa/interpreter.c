//
// Created by jonas on 24.11.15.
//

#include "interpreter.h"

void expandDataSegment (unsigned char **data_segment, size_t *data_length);

void processLoop(InterpreterArguments *interpreter_arguments, int direction);

int jumpToMatchingBrace(InterpreterArguments *interpreter_arguments);

void insertJump(Jumps *jumps, Jump jump);


int interpreter (InterpreterArguments *interpreter_arguments)
{
  // non static variables
  const int direction = (interpreter_arguments->steps_ < 0) ? -1 : 1;

  const char move_forward = (direction == 1) ? (char)'>' : (char)'<';
  const char move_back    = (direction == 1) ? (char)'<' : (char)'>';
  const char increment    = (direction == 1) ? (char)'+' : (char)'-';
  const char decrement    = (direction == 1) ? (char)'-' : (char)'+';
  // print and read do not change
  // the loops are more complex
  const char loop_start   = (direction == 1) ? (char)'[' : (char)']';
  const char loop_end     = (direction == 1) ? (char)']' : (char)'[';

  int break_loop = 0;

  //create a local variable for steps because it should not be changed
  //if -1 runs infinitely long (to the end of the program)
  int steps = (interpreter_arguments->steps_ == 0) ? -1 : interpreter_arguments->steps_;

  //the program must run shifted one to the left, increment at the end
  if(direction == -1)
    interpreter_arguments->program_counter_--;

  for (; *(interpreter_arguments->program_counter_) != 0 && steps != 0;
         steps -= direction)
  {
    //check if a breakpoint is reached
    int *breakpoint = interpreter_arguments->breakpoints_;
    for (; breakpoint < interpreter_arguments->breakpoints_ + interpreter_arguments->breakpoint_count_; breakpoint++)
    {
      if (interpreter_arguments->program_ + *breakpoint == interpreter_arguments->program_counter_)
      {
        break_loop = 1;
      }
    }
    if (break_loop)
      break;

    //printf("%lu %c: ", interpreter_arguments->program_counter_ - interpreter_arguments->program_, *(interpreter_arguments->program_counter_));

    // ------------  >  ------------
    if (*(interpreter_arguments->program_counter_) == move_forward)
    {
      if (++(*interpreter_arguments->data_pointer_) >
          *interpreter_arguments->data_segment_ + *(interpreter_arguments->data_length_))
      {
        expandDataSegment(interpreter_arguments->data_segment_, interpreter_arguments->data_length_);
      }
      interpreter_arguments->program_counter_ += direction;
    }


    // ------------  <  ------------
    else if (*(interpreter_arguments->program_counter_) == move_back)
    {
      if (--(*interpreter_arguments->data_pointer_) < *interpreter_arguments->data_segment_)
      {
        (*interpreter_arguments->data_pointer_)++;
      }

      interpreter_arguments->program_counter_ += direction;
    }


    // ------------  +  ------------
    else if (*(interpreter_arguments->program_counter_) == increment)
    {
      (**interpreter_arguments->data_pointer_)++;

      interpreter_arguments->program_counter_ += direction;
    }


    // ------------  -  ------------
    else if (*(interpreter_arguments->program_counter_) == decrement)
    {
      (**interpreter_arguments->data_pointer_)--;

      interpreter_arguments->program_counter_ += direction;
    }


    // ------------  .  ------------
    else if (*(interpreter_arguments->program_counter_) == '.')
    {
      putchar(**interpreter_arguments->data_pointer_);

      interpreter_arguments->program_counter_ += direction;
    }


    // ------------  .  ------------
    else if (*(interpreter_arguments->program_counter_) == ',')
    {
      **interpreter_arguments->data_pointer_ = (unsigned char) getchar ();

      interpreter_arguments->program_counter_ += direction;
    }


    // ------------  [  ------------
    else if (*(interpreter_arguments->program_counter_) == loop_start)
    {
      processLoop(interpreter_arguments, direction);
    }


    // ------------  ]  ------------
    else if (*(interpreter_arguments->program_counter_) == loop_end)
    {
      processLoop(interpreter_arguments, direction);
    }

    interpreter_arguments->step_counter_ += direction;

    //printf(" step: %d\n", interpreter_arguments->step_counter_-1);
  }

  if(direction == -1)
    interpreter_arguments->program_counter_++;

  if (steps == 0) //program ran the specified steps to the end
  {
    return STEP_STOP;
  }

  if (break_loop)
  {
    return BREAKPOINT_STOP;
  }

  return REGULAR_STOP;
}

void resetInterpreterArguments(InterpreterArguments *interpreter_arguments)
{
  interpreter_arguments->step_counter_ = 0;
  interpreter_arguments->jumps_.count_ = 0;

  memset (*interpreter_arguments->data_segment_, 0,
          *interpreter_arguments->data_length_ * sizeof(unsigned char));

  *interpreter_arguments->data_pointer_ = *interpreter_arguments->data_segment_;

  interpreter_arguments->breakpoint_count_ = 0;
}


InterpreterArguments getUsableInterpreterArgumentsStruct(
  unsigned char **data_segment, size_t *data_length,
  unsigned char **data_pointer)
{
  InterpreterArguments interpreter_arguments = {
    NULL,
    0,
    data_segment,
    data_length,
    NULL,
    data_pointer,
    0,
    NULL,
    0,
    0,
    {0, 0, NULL}
  };

  if(data_segment == NULL)
  {
    interpreter_arguments.data_length_ = malloc(sizeof(size_t));
    *interpreter_arguments.data_length_ = 1024;

    interpreter_arguments.data_segment_ = malloc(sizeof(unsigned char*));
    *interpreter_arguments.data_segment_ =
      calloc(*interpreter_arguments.data_length_, sizeof(unsigned char));

    interpreter_arguments.data_pointer_ = malloc(sizeof(unsigned char*));
    *interpreter_arguments.data_pointer_ = *interpreter_arguments.data_segment_;
  }

  interpreter_arguments.jumps_.allocated_memory_ = 100;
  interpreter_arguments.jumps_.array_ = (Jump *) malloc(100 * sizeof(Jump));

  return interpreter_arguments;
}




void expandDataSegment (unsigned char **data_segment, size_t *data_length)
{
  *data_length *= 2;
  *data_segment = realloc (*data_segment, *data_length);

  //set the new memory to 0
  memset (*data_segment + (*data_length) / 2, 0, (*data_length) / 2);
}



void processLoop(InterpreterArguments *interpreter_arguments, int direction)
{
  if (direction == 1)
  {
    if (
        (
          *(interpreter_arguments->program_counter_) == '[' &&
          **interpreter_arguments->data_pointer_ == 0
        )
        ||
        (
          *(interpreter_arguments->program_counter_) == ']' &&
          **interpreter_arguments->data_pointer_ != 0
        )
      )
    {
      int distance = jumpToMatchingBrace(interpreter_arguments);
      insertJump(&interpreter_arguments->jumps_,
                 (Jump){interpreter_arguments->step_counter_, distance});
    }
    else
    {
      interpreter_arguments->program_counter_++;
    }
  }
  else if (direction == -1)
  {
    //search in the jumps array if the program was jumping
    int search_loop_index;
    int jump_found = 0;

    //search from back because, the chance that the jump is at the end is high
    for (search_loop_index = interpreter_arguments->jumps_.count_;
         search_loop_index > 0; search_loop_index++)
    {
      //if the program was jumping
      if (interpreter_arguments->jumps_.array_[search_loop_index].step_ ==
        interpreter_arguments->step_counter_ - 1)
      {
        //jump in the opposite direction
        interpreter_arguments->program_counter_ -=
          interpreter_arguments->jumps_.array_[search_loop_index].distance_;

        jump_found = 1;

        //printf("Program was jumping on step %d, jumping back by %d", jumps->array_[search_loop_index].step_, -jumps->array_[search_loop_index].distance_);
      }
    }

    if (jump_found == 0)
      interpreter_arguments->program_counter_--;
  }
}


int jumpToMatchingBrace(InterpreterArguments *interpreter_arguments)
{
  int loop_counter = 1;
  int distance = 0;
  int direction = (*(interpreter_arguments->program_counter_) == '[') ? 1 : -1;

  distance += direction;
  interpreter_arguments->program_counter_ += direction;

  while (loop_counter > 0)
  {
    if (*(interpreter_arguments->program_counter_) == '[')
    {
      loop_counter += direction;
    }
    else if (*(interpreter_arguments->program_counter_) == ']')
    {
      loop_counter -= direction;
    }

    distance += direction;
    interpreter_arguments->program_counter_ += direction;
  }

  if(direction == -1)
  {
    interpreter_arguments->program_counter_ += 2;
    distance += 2;
  }

  return distance;
}


void insertJump(Jumps *jumps, Jump jump)
{
  jumps->array_[jumps->count_] = jump;

  jumps->count_++;

  if(jumps->count_ == jumps->allocated_memory_)
  {
    //extend array
    jumps->allocated_memory_ *= 2;
    jumps->array_ = realloc(jumps->array_, jumps->allocated_memory_);
  }

  //printf("Program is jumping on step %d by %d", jump.step_, jump.distance_);
}