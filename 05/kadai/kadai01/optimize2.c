#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "optimize2.h"

double calc_norm(const int dim, double v[])
{
   double tmp = 0;
   int    i   = 0;
   for(i = 0; i < dim; i++) {
      tmp += v[i] * v[i];
   }
   tmp = sqrt(tmp);
   return tmp;
}

int optimize(const double alpha, const int dim, double x[], void (*calc_grad)(const double[], double[]),
             double (*calc_val)(const double[]))
{
   int i;

   double* g = malloc(dim * sizeof(double));

   int iter = 0;
   while(++iter < 10000) {
      (*calc_grad)(x, g);

      const double norm = calc_norm(dim, g);
      printf("%3d norm = %e", iter, norm);
      for(i = 0; i < dim; i++) {
         printf(", x[%d] = %13.9f", i, x[i]);
      }
      printf(", f = %13.9f", (*calc_val)(x));
      putchar('\n');

      if(norm < 1e-10) {
         break;
      }

      for(i = 0; i < dim; i++) {
         x[i] -= alpha * g[i];
      }
   }

   free(g);

   return iter;
}
