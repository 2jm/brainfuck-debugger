//
// Created by John on 03.12.2015.
// Bonus Eso Language (Simple one)
// http://esolangs.org/wiki/BIO
//

#include <stdio.h>
#include <stdlib.h>

#define NOT_LOADED       0
#define LOADED_FROM_FILE 1

typedef struct Loop_
{
  char *data_ptr_;
  struct Loop_ *next;
  int cell_;
} Loop;

int interpret(unsigned char *memory, char *program, char *program_ctr);

int load(char *path, char **program, int program_size);

int getNextCmd(char **program_ctr, int *cell);

void push(Loop **top, char *data_ptr, int *cell);

char *pop(Loop **top);


int main(int argc, char *argv[])
{
  int program_loaded = NOT_LOADED;

  if (argc > 1)
  {
    char *path = argv[1];
    int program_size = 100;
    char *program = malloc(program_size * sizeof(char));
    char *program_ctr = malloc(sizeof(char *));
    size_t memory_blocks = 3;
    unsigned char *memory = calloc(memory_blocks, sizeof(unsigned char *));
    program_loaded = load(path, &program, program_size);
    if (program_loaded == NOT_LOADED)
    {
      return 2;
    }
    return interpret(memory, program, program_ctr);
  }
  else
  {
    return 1;
  }
}

void push(Loop **top, char *data_ptr, int *cell)
{
  // make new stack item and copy data to it:
  Loop *new_item = malloc(sizeof(Loop));
  new_item->data_ptr_ = data_ptr;
  new_item->cell_ = *cell;

  new_item->next = *top;    // next points to previous top
  *top = new_item;          // top now points to new item
}

char *pop(Loop **top)
{
  Loop *old_top = *top;       // remember the old top
  char *data_ptr = old_top->data_ptr_;   // remember the old data
  *top = old_top->next;       // move top down
  free(old_top);              // now we can free the old StackItem
  return data_ptr;            // and return the data we remembered
}

int interpret(unsigned char *memory, char *program, char *program_ctr)
{
  int *cell = calloc(1, sizeof(int));
  Loop *loop_stack = NULL;

  for (program_ctr = program; *program_ctr != '\0'; program_ctr++)
  {
    if (*(program_ctr + 1) == '\0')
    {
      break;
    }
    int cmd = getNextCmd(&program_ctr, cell);
    if (cmd == -1)
    {
      //failed
      return -1;
    }
    else if (cmd == 0)
    {
      memory[*cell]++;
    }
    else if (cmd == 1)
    {
      memory[*cell]--;
    }
    else if (cmd == 2)
    {
      if (memory[*cell] == 0)
      {
        // go to matching end bracket
        int found_right = 1;
        while(found_right != 0)
        {
          program_ctr++;
          if(*program_ctr == '}')
          {
            found_right--;
          }
          else if(*program_ctr == '{')
          {
            found_right++;
          }
        }
        program_ctr--;
      }

      // push start position of loop
      push(&loop_stack, program_ctr, cell);
    }
    else if (cmd == 3)
    {
      //if (memory[*cell] != 0)
      if(memory[loop_stack->cell_] != 0)
      {
        if (loop_stack == NULL)
        {
          // too much closing brackets
          return -1;
        }
        program_ctr = loop_stack->data_ptr_;
      }
      else
      {
        pop(&loop_stack);
      }
    }
    else if (cmd == 4)
    {
      putchar(memory[*cell]);
    }
  }
  return 0;
}

int getNextCmd(char **program_ctr, int *cell)
{
  int ret = -1;
  char first = **program_ctr;
  char second = *(*program_ctr + 1);

  if (first == '}' && second == ';')
  {
    (*program_ctr)++;
    return 3;
  }
  if (second == 'o')
  {
    if (first == '0')
    {
      ret = 0; // Increment the specified block
    }
    else if (first == '1')
    {
      ret = 1; // Decrement the specified block
    }
    // get the correct cell (either x -> 0, y -> 1, z -> 2)
    *cell = *(*program_ctr + 2) - 'x';
    if (0 <= *cell && *cell <= 2 && *(*program_ctr + 3) == ';')
    {
      //0ox;  0oy;  0oz;
      *program_ctr += 3;
    }
    else
    {
      ret = -1;
    }
  }
  else if (second == 'i')
  {
    if (first == '0')
    {
      ret = 2; // While the block is not 0
    }
    else if (first == '1')
    {
      ret = 4; // Output the block
    }
    // get the correct cell (either x -> 0, y -> 1, z -> 2)
    *cell = *(*program_ctr + 2) - 'x';
    if (0 <= *cell && *cell <= 2)
    {
      if (!((ret == 2 && *(*program_ctr + 3) == '{') || (ret == 4 && *(*program_ctr + 3) == ';')))
      {
        ret = -1;
      }
      //0ox;  0oy;  0oz;  Oix{  etc.
      *program_ctr += 3;
    }
    else
    {
      ret = -1;
    }
  }
  return ret;
}

int load(char *path, char **program, int program_size)
{
  FILE *file;

  if ((file = fopen(path, "r")) == NULL)
  {
    printf("[ERR] reading the file failed\n");
    return NOT_LOADED;
  }

  int ctr; // has to be a integer to read EOF as -1
  int pos = 0;
  *program = malloc(program_size * sizeof(char));

  int comment = 0;

  //read the file char by char and write it into an array
  while ((ctr = fgetc(file)) != EOF)
  {
    if (comment == 1)
    {
      if (ctr == '\n')
      {
        //comment ended
        comment = 0;
      }
      continue;
    }
    if (ctr == '/')
    {
      if ((ctr = fgetc(file)) == EOF)
      {
        break;
      }
      else
      {
        if (ctr == '/')
        {
          // double "/" = comment
          comment = 1;
          continue;
        }
      }
    }
    if (ctr == '0' || ctr == '1' || ctr == 'o' || ctr == 'i' || ctr == 'x' ||
        ctr == 'y' || ctr == 'z' || ctr == '{' || ctr == '}' || ctr == ';')
    {
      // get more program memory if the file is too long
      if (pos == program_size)
      {
        program_size *= 2;
        *program = realloc(*program, program_size * sizeof(char));
      }

      (*program)[pos++] = (char) ctr;
    }
  }
  (*program)[pos] = '\0';  //end of string

  if ((*program)[pos - 1] != ';')
  {
    printf("[ERR] parsing of input failed\n");
  }

  fclose(file);

  return LOADED_FROM_FILE;
}