//
// Created by John on 03.12.2015.
// Bonus Eso Language (Simple one)
// http://esolangs.org/wiki/BIO
//

#include <stdio.h>
#include <stdlib.h>

#define NOT_LOADED       0
#define LOADED_FROM_FILE 1

int interpret(unsigned char *memory, char *program, char *program_ctr);
int load(char *path, char **program, int program_size);
int getNextCmd(char **program_ctr, int *cell);

int main(int argc, char *argv[])
{
  int program_loaded = NOT_LOADED;

  if (argc > 1)
  {
    char *path = argv[1];
    int program_size = 100;
    char *program = malloc(program_size * sizeof(char));
    char *program_ctr = malloc(sizeof(char *));
    int memory_blocks = 3;
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

int interpret(unsigned char *memory, char *program, char *program_ctr)
{
  int *cell = calloc(1, sizeof(int));
  int looping = 0;
  char *loop_ptr = malloc(sizeof(char*));

  for (program_ctr = program; *program_ctr != '\0'; program_ctr++)
  {
    // TODO: remove debug code:
    //char test = *program_ctr;
    //putchar(test);

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
      continue;
    }
    else if (cmd == 1)
    {
      memory[*cell]--;
      continue;
    }
    else if (cmd == 2)
    {
      // TODO: change to proove if cell is 0 here?
      // TODO: add code for nested loops, otherwise 99 bottles of beer is not working
      looping = 1;
      loop_ptr = program_ctr;
      continue;
    }
    else if(cmd == 3)
    {
      // failed
      if(!looping)
      {
        return -1;
      }
      if(memory[*cell] != 0)
      {
        program_ctr = loop_ptr;
      }
      else
      {
        continue;
      }
    }
    else if (cmd == 4)
    {
      putchar(memory[*cell]);
      continue;
    }
  }
  return 0;
}

int getNextCmd(char **program_ctr, int *cell)
{
  int ret = -1;
  char first = **program_ctr;
  char second = *(*program_ctr + 1);

  if(first == '}' && second == ';')
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
      if(ret == 2 && *(*program_ctr + 3) == '{')
      {

      }
      else if(ret == 4 && *(*program_ctr + 3) == ';')
      {

      }
      else
      {
        ret = -1;
      }
      //0ox;  0oy;  0oz;
      *program_ctr += 3;
    }
    else
    {
      ret = -1;
    }
  }
  // failed
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