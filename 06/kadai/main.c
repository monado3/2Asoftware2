#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include "encode.h"

int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "error: please specify the file.\n");
    exit(1);
  }

  encode(argv[1]);

  return 0;
}
