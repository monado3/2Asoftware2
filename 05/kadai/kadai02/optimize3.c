#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "optimize3.h"

// 乱数を使ってランダムにミニバッチの集合を作成
void make_part(int[], int);

// copy each vector component from vec2 to vec1
void vector_copy(double[], double[], int);

extern double** Gdata;

double calc_norm(const int dim, double v[])
{
   double tmp = 0;
   int    i   = 0;
   for(i = 0; i < dim; i++) {
      tmp += pow(v[i], 2);
   }
   tmp = sqrt(tmp);
   return tmp;
}

int optimize(const double alpha, const int dim, const int nsamples, double x[],
             void (*calc_grad)(const double[], double[], const int[], const int, const int, const int),
             double (*calc_val)(const double[], const int, const int))
{
   int     i;
   int     startidx;
   double* g        = (double*) malloc(dim * sizeof(double));
   double* prevdx   = (double*) malloc(dim * sizeof(double));
   int*    part     = (int*) malloc(nsamples * sizeof(int));

   for(int i = 0; i < dim; i++) {
      prevdx[i] = 0.0;
   }

   // debug
   // for(int row = 0; row < nsamples; row++) {
   //    printf("data No.%3d = ( ", row + 1);
   //    for(int col = 0; col < dim+1; col++) {
   //       printf("%9f, ", Gdata[row][col]);
   //    }
   //    printf("\b\b )\n");
   // }
   // printf("\n");
   int    iter = 0;
   double gradx;
   double dx;
   double norm;

   while(++iter < 4e5) {
      startidx = 0;
      make_part(part, nsamples);
      while(startidx < nsamples) {
         (*calc_grad)(x, g, part, nsamples, dim, startidx);
         iter++;
         for(i = 0; i < dim; i++) {
            // g[i] += prevdx[i];
            gradx       = (alpha / pow(iter, 0.4)) * g[i];
            dx          = gradx + 0.91 * prevdx[i];
            x[i]       -= dx;
            prevdx[i]   = dx;
            // x[i] -= alpha * g[i] * 1e4 * exp(-pow(iter-1e6,2)/1e150);
         }
         norm = calc_norm(dim, g);
         if(norm < 1) {
            break;
         }
         startidx += 4;
      }

      // debug
      // printf("x[0] = %f ", x[0]);
      // printf("g[0] = %f ", g[0]);
      // printf("x[3] = %f ", x[3]);

      if(iter % (int) 1e4 < 25) {
         printf("%e norm = %e\n", (double) iter, norm);
         // printf("  g[3] = %e\n", g[3]);
         for(i = 0; i < dim; i++) {
            printf("x[%2d] = %14.4f", i, x[i]);
            printf(", g[%2d] = %e\n", i, g[i]);
         }
         printf("f = %e\n", (*calc_val)(x, nsamples, dim));
         putchar('\n');
      }
   }

   free(g);
   free(prevdx);
   free(part);

   return iter;
}

void make_part(int part[], int nsamples) // 乱数を使ってランダムにミニバッチの集合を作成
{
   part[0] = 0;
   int i = 0;
   int option;
   while(i < nsamples) {
      option = rand() % nsamples;
      int checker = 0;
      for(int index = 0; index < i; index++) {
         if(part[index] == option) {
            checker = 1; break;
         }
      }
      if(checker == 1) {
         continue;
      }
      part[i] = option;
      i++;
   }
}

void vector_copy(double vec1[], double vec2[], int dim)
{
   // copy each vector component from vec2 to vec1
   for(int row = 0; row < dim; row++) {
      vec1[row] = vec2[row];
   }
}
