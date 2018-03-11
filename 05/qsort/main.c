#include <stdio.h>
#include <stdlib.h>
#include "qsort.h"

int main()
{
   int       v[] = { 13, 4, 9, 15, 7 };
   const int n   = sizeof(v) / sizeof(v[0]);

   qsort(v, n, sizeof(v[0]), comp_int);

   int i;
   for(i = 0; i < n; i++) {
      printf("v[%d] = %d\n", i, v[i]);
   }

   return 0;
}
