#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#define WIDTH      70
#define HEIGHT     40
#define MAX_CITIES 1000

typedef struct
{
   int x;
   int y;
} City;

City cities[MAX_CITIES];
char map[WIDTH][HEIGHT];

int max(const int a, const int b)
{
   return (a > b) ? a : b;
}

void draw_line(const int x0, const int y0, const int x1, const int y1)
{
   int       i;
   const int n = max(abs(x1 - x0), abs(y1 - y0)) + 1;
   for(i = 0; i <= n; i++) {
      const int x = x0 + i * (x1 - x0) / n;
      const int y = y0 + i * (y1 - y0) / n;
      if(map[x][y] == ' ') {
         map[x][y] = '*';
      }
   }
}

void draw_route(const int n, const int* route)
{
   if(route == NULL) {
      return;
   }

   int i;
   for(i = 0; i < n; i++) {
      const int c0 = route[i];
      const int c1 = (i < n - 1) ? route[i + 1] : route[0];
      draw_line(cities[c0].x, cities[c0].y, cities[c1].x, cities[c1].y);
   }
}

void plot_cities(FILE* fp, const int n, const int* route)
{
   int i, j, x, y;

   fprintf(fp, "----------\n");

   memset(map, ' ', sizeof(map));

   for(i = 0; i < n; i++) {
      char buf[100];
      sprintf(buf, "C_%d", i);
      for(j = 0; j < strlen(buf); j++) {
         const int x = cities[i].x + j;
         const int y = cities[i].y;
         map[x][y] = buf[j];
      }
   }

   draw_route(n, route);

   for(y = 0; y < HEIGHT; y++) {
      for(x = 0; x < WIDTH; x++) {
         const char c = map[x][y];
         fputc(c, fp);
      }
      fputc('\n', fp);
   }

   fflush(fp);
}

double distance(const int i, const int j)
{
   const double dx = cities[i].x - cities[j].x;
   const double dy = cities[i].y - cities[j].y;
   return sqrt(dx * dx + dy * dy);
}

void make_initsol(int initsol[], int n) // 乱数を使って完全にランダムに調査
{
   initsol[0] = 0;
   int i = 1;
   int option;
   while(i < n) {
      option = (rand() % (n - 1)) + 1;
      int checker = 0;
      for(int index = 1; index < i; index++) {
         if(initsol[index] == option) {
            checker = 1; break;
         }
      }
      if(checker == 1) {
         continue;
      }
      initsol[i] = option;
      i++;
   }
}

double routedis(int n, int route[])
{
   double sum_d = 0.0;
   for(int i = 0; i < n; i++) {
      const int c0 = route[i];
      const int c1 = route[(i + 1) % n];
      sum_d += distance(c0, c1);
   }
   return sum_d;
}

void copyarray(int array1[], int array2[], int n) // copy from array2 to array1
{
   for(int i = 0; i < n; i++) {
      array1[i] = array2[i];
   }
}

double solve(const int n, int ansroute[])
{
   int*   initsol;
   int*   improvedsol;
   int*   replacesol;
   int    temp;
   double ans = 1e6;
   double tempvalue;
   int    checker;
   double minofthissol;
   initsol     = (int*) malloc(sizeof(int) * n);
   improvedsol = (int*) malloc(sizeof(int) * n);
   replacesol  = (int*) malloc(sizeof(int) * n);
   for(int numsol = 0; numsol < n*n; numsol++) {  // n個のランダム初期解について調査
      make_initsol(initsol, n);
      tempvalue    = routedis(n, initsol);
      minofthissol = tempvalue;
      if(tempvalue < ans) {
         ans = tempvalue;
         copyarray(ansroute, initsol, n);
      }
      checker = 1;
      copyarray(improvedsol, initsol, n);
      while(checker) {
         checker = 0;
         copyarray(initsol, improvedsol, n);
         for(int i1 = 1; i1 < n - 1; i1++) {
            for(int i2 = i1 + 1; i2 < n; i2++) {
               copyarray(replacesol, initsol, n);
               temp = replacesol[i1];
               replacesol[i1] = replacesol[i2];
               replacesol[i2] = temp;
               tempvalue      = routedis(n, replacesol);
               if(tempvalue < minofthissol) {
                  minofthissol = tempvalue;
                  copyarray(improvedsol, replacesol, n);
                  checker = 1;
                  if(tempvalue < ans) {
                     ans = tempvalue;
                     copyarray(ansroute, replacesol, n);
                  }
               }
            }
         }
      }
   }

   free(initsol);
   free(replacesol);
   free(improvedsol);

   return ans;
}

int main(const int argc, const char** argv)
{
   FILE*       fp;
   const char* map_file = "map.txt";
   if((fp = fopen(map_file, "a")) == NULL) {
      fprintf(stderr, "error: cannot open %s.\n", map_file);
      return 1;
   }

   if(argc != 2) {
      fprintf(stderr, "error: please specify the number of cities.\n");
      exit(1);
   }

   const int n = atoi(argv[1]);
   assert(n > 1 && n <= MAX_CITIES);

   int i;
   for(i = 0; i < n; i++) {
      cities[i].x = rand() % (WIDTH - 5);
      cities[i].y = rand() % HEIGHT;
   }

   plot_cities(fp, n, NULL);
   sleep(1);

   int ansroute[MAX_CITIES];
   const double d = solve(n, ansroute);

   printf("total distance = %f\n", d);
   plot_cities(fp, n, ansroute);

   return 0;
}
