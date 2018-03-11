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
int  ansroute[MAX_CITIES];

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

double solve(const int n, int visitnum, int route[])
{
   int i;

   // Compute the total distance
   if(visitnum == n) {
      double sum_d = 0;
      for(i = 0; i < n; i++) {
         const int c0 = route[i];
         const int c1 = route[(i + 1) % n];
         sum_d += distance(c0, c1);
      }
      return sum_d;
   }

   double ans = 1e6;
   double temp;

   route[0] = 0;                                                     // Start from cities[0]
   for(int nextcity = 1; nextcity < n; nextcity++) {                 // 次に行く候補をcity1からcity(n-1)までチェック
      int checker = 0;
      for(int routeindex = 1; routeindex < visitnum; routeindex++) { // routeで通ったことがないかチェック
         if(route[routeindex] == nextcity) {
            checker = 1; break;
         }
      }
      if(checker == 1) {
         continue;
      }
      route[visitnum] = nextcity;
      temp            = solve(n, visitnum + 1, route);
      if(temp < ans) {
         ans = temp;
         for(i = 0; i < n; i++) {
            ansroute[i] = route[i];
         }
      }
   }

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

   int route[MAX_CITIES];
   const double d = solve(n, 1, route);

   printf("total distance = %f\n", d);
   plot_cities(fp, n, ansroute);

   return 0;
}
