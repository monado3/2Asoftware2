#include <stdio.h>
#include <math.h>

extern double** Gdata;

double f_value(const double x[], const int nsamples, const int dim)
{
   double value = 0;
   double tmp;
   for(int sample = 0; sample < nsamples; sample++) {
      tmp = 0.0;
      for(int i = 0; i < dim; i++) {
         tmp += x[i] * Gdata[sample][i + 1];
      }
      value += pow(Gdata[sample][0] - tmp, 2);
   }
   return value;
}

void f_gradient(const double x[], double g[], const int part[], const int nsamples, const int dim, const int startidx)
{
   double tmp;

   for(int i = 0; i < dim; i++) {
      g[i] = 0.0;
   }
   for(int i1 = 0; i1 < dim; i1++) {
      for(int sample = startidx; sample < startidx + 4; sample++) {
         tmp = 0.0;
         for(int i2 = 0; i2 < dim; i2++) {
            tmp += x[i2] * Gdata[part[sample % nsamples]][i2 + 1];
         }
         g[i1] += -2 * (Gdata[part[sample % nsamples]][0] - tmp) * Gdata[part[sample % nsamples]][i1 + 1];
      }
   }
}
