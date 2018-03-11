#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "optimize3.h"
#include "regression.h"

// when reading a line of a file, the upper limit to the number of read characters
#define MAXLENOFLINE 2000

// input data from the file
void input_data(FILE*, int*, int*);

// make a data array from the file
void make_dataarray(FILE*, int);

double** Gdata;

int main(const int argc, const char** argv)
{
   FILE* fp;
   double*      base_Gdata;
   int          rows = -1;
   int          cols = 1;
   int          row;
   char         filename[] = "kadai02/houserent_data.csv";
   const double alpha      = (argc == 2) ? atof(argv[1]) : 1e-3;

   if((fp = fopen(filename, "r")) == NULL) {
      printf("An error was encountered while reading the file.\n");

      return 1;
   }

   input_data(fp, &rows, &cols);   // input data from the file

   Gdata      = (double**) malloc(sizeof(double*) * rows);
   base_Gdata = (double*) malloc(sizeof(double) * rows * cols);
   for(row = 0; row < rows; row++) {
      Gdata[row] = base_Gdata + row * cols;
   }

   make_dataarray(fp, rows);

   int       i;
   const int dim      = cols - 1;
   const int nsamples = rows;

   double* x = (double*)malloc(dim * sizeof(double));
   for(i = 0; i < dim; i++) {
      x[i] = 0.0;
   }

   printf("alpha = %f\n", alpha);

   // for(row = 0; row < rows; row++) {
   //    printf("data No.%3d = ( ", row + 1);
   //    for(int col = 0; col < cols; col++) {
   //       printf("%9f, ", Gdata[row][col]);
   //    }
   //    printf("\b\b )\n");
   // }
   // printf("\n");

   const int iter = optimize(alpha, dim, nsamples, x, f_gradient, f_value);

   printf("number of iterations = %d\n", iter);

   printf("houserent = ");
   for(int i = 0; i < dim - 1; i++) {
      printf("%7.4f * x_%d + ", x[i], i + 1);
   }
   printf("%7.4f\n", x[dim - 1]);

   // printf("temperature = %14.9f * altitude + %14.9f\n", x[0], x[1]);

   free(x);
   free(Gdata);
   free(base_Gdata);

   return 0;
}

void input_data(FILE* fp, int* rows, int* cols)
{
   // input data from file
   char*  bufchar;
   double bufnum;
   char   temp[MAXLENOFLINE];

   while(fgets(temp, MAXLENOFLINE, fp) != NULL) {
      (*rows)++;
   }

   bufnum = atof(bufchar = strtok(temp, " \t,"));
   if(((bufnum = atof(bufchar)) == 0) && *bufchar != '0') {
   } else {
      (*cols)++;
   }

   while((bufchar = strtok(NULL, " \t,")) != NULL) {
      if(((bufnum = atof(bufchar)) == 0) && *bufchar != '0') {
         continue;
      } else {
         (*cols)++;
      }
   }

   fseek(fp, 0, SEEK_SET);
}

void make_dataarray(FILE* fp, int rows)
{
   // make a data array from file
   int    row, col;
   char*  bufchar;
   double bufnum;
   char   temp[MAXLENOFLINE];

   fgets(temp, MAXLENOFLINE, fp);
   for(row = 0; row < rows; row++) {
      fgets(temp, MAXLENOFLINE, fp);
      bufnum = atof(bufchar = strtok(temp, " \t,")); // 'comma', 'space', and 'tab' as delimiters.
      if((bufnum = atof(bufchar)) == 0 && *bufchar != '0') {
         col = 0;
      } else {
         Gdata[row][0] = bufnum;
         col = 1;
      }
      while((bufchar = strtok(NULL, " \t,")) != NULL) {
         if(((bufnum = atof(bufchar)) == 0) && *bufchar != '0') {
            continue;
         } else {
            Gdata[row][col] = bufnum;
            col++;
         }
      }
      Gdata[row][col] = 1.0;
   }
}
