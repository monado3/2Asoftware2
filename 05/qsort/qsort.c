#include <stdio.h>
#include <stdlib.h>

int comp_int(const void *x0, const void *x1)
{
  const int *p0 = x0; // x0 is interpreted as a pointer to an integer
  const int y0 = *p0; // y0 is the int value that exists at p0
  //  const int y0 = *(int*)x0;  // The above can be written in one line.

  const int *p1 = x1;
  const int y1 = *p1;
  //  const int y1 = *(int*)x1;

  if (y0 > y1) return  1;
  if (y0 < y1) return -1;
  return 0;
}

int main()
{
  int v[] = { 13, 4, 9, 15, 7};
  const int n = sizeof(v) / sizeof(v[0]);

  qsort(v, n, sizeof(v[0]), comp_int);

  int i;
  for (i = 0; i < n; i++) {
    printf("v[%d] = %d\n", i, v[i]);
  }

  return 0;
}
