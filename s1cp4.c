#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "helper.h"


int main(void)
{
  __debug__ = 0;

 	char *line;
  char *filename = "4.txt";
  unsigned char isenc = 0, key = 0;
  size_t size = 0;
  ssize_t readsize = 0;
  FILE *f;
  int linenum = 0;

  f = fopen(filename, "r");
  if (!f) {
    err("Invalid filename %s", filename);
    return -1;
  }

  while ((readsize = getline(&line, &size, f)) > 0) {
    if (line[readsize - 1] == '\n')
        line[readsize - 1] = '\0';
    get_single_key_xor(line, &isenc, &key);
    if (isenc)
      info("line number = %d", ++linenum);
    size = 0;
    free(line);
  }
  fclose(f);
	//printf("Input  = %s\n", ip);

}