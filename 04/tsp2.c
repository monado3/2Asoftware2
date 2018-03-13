/*シミュレーテッドアニーリング法を何回も試行するアルゴリズムを採用しました。
 * このアルゴリズムではアニーリングの試行回数が多いほど結果が最適解に近づきやすくなるので
 * 任意の2つの街の距離は予め計算し配列に確保しておき、また高速な乱数関数としてxorshiftを採用しました。
 * n = 20の場合はおそらく最適解です。nの20近傍でも最適解が出やすいようにパラメータを調整しました。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define WIDTH      70
#define HEIGHT     40
#define MAX_CITIES 1000
#define EPSILON    1e-100
#define COOLRATE   0.95

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

unsigned long xor128()
{
   static unsigned long x = 123456789, y = 362436069, z = 521288629, w = 88675123;
   unsigned long t;
   t = (x ^ (x << 11)); x = y; y = z; z = w;
   return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

void make_initsol(int initsol[], int n) // 乱数を使って完全にランダムに調査
{
   initsol[0] = 0;
   int i = 1;
   int option;
   while(i < n) {
      option = (xor128() % (n - 1)) + 1;
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

double routedis(int n, int route[], double** distances)
{
   double sum_d = 0.0;
   for(int i = 0; i < n; i++) {
      const int c0 = route[i];
      const int c1 = route[(i + 1) % n];
      sum_d += distances[c0][c1];
   }
   return sum_d;
}

void copyarray(int array1[], int array2[], int n) // copy from array2 to array1
{
   for(int i = 0; i < n; i++) {
      array1[i] = array2[i];
   }
}

void random2offset(int changeidx[], int n)
{
   changeidx[0] = (xor128() % (n - 1)) + 1;
   int option;
   while(1) {
      option = (xor128() % (n - 1)) + 1;
      if(option  !=  changeidx[0]) {
         changeidx[1] = option; break;
      }
   }
}

double solve(const int n, int ansroute[], double** distances) // simulated annealing
{
   int*   initsol;
   int*   improvedsol;
   int*   replacesol;
   int    temp;
   double tempans;
   double ans = 1e10;
   double tempvalue;
   double temperature;
   int    changeidx[2];
   initsol     = (int*) malloc(sizeof(int) * n);
   improvedsol = (int*) malloc(sizeof(int) * n);
   replacesol  = (int*) malloc(sizeof(int) * n);
   int counter = 0;

   for(int simulate = 0; simulate < 3000; simulate++) {
      make_initsol(initsol, n);
      copyarray(improvedsol, initsol, n);
      temperature = 50;
      tempans     = 1e50;
      while(temperature > EPSILON) {
         copyarray(replacesol, improvedsol, n);
         random2offset(changeidx, n);
         temp = replacesol[changeidx[0]];
         replacesol[changeidx[0]] = replacesol[changeidx[1]];
         replacesol[changeidx[1]] = temp;
         tempvalue = routedis(n, replacesol, distances);
         if(((double) rand() / RAND_MAX) <= exp((tempans - tempvalue) / temperature)) {
            tempans = tempvalue;
            copyarray(improvedsol, replacesol, n);
         }
         temperature *= COOLRATE;
         counter++;
      }
      if(tempans < ans) {
         ans = tempans;
         copyarray(ansroute, improvedsol, n);
      }
   }
   printf("%d\n", counter);

   free(initsol);
   free(replacesol);
   free(improvedsol);

   return ans;
}

void make_dis_list(double** distances, int n)
{
   for(int row = 0; row < n; row++) {
      for(int col = 0; col < n; col++) {
         distances[row][col] = distance(row, col);
      }
   }
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


   double** distances; //同じ計算を何回もするのは無駄なのであらかじめ表にしておく
   double*  base_distances;
   distances      = (double**) malloc(sizeof(double*) * n);
   base_distances = (double*) malloc(sizeof(double) * n * n);
   for(int row = 0; row < n; row++) {
      distances[row] = base_distances + row * n;
   }

   make_dis_list(distances, n);

   int ansroute[MAX_CITIES];
   const double d = solve(n, ansroute, distances);

   printf("total distance = %f\n", d);
   plot_cities(fp, n, ansroute);

   free(base_distances);
   free(distances);

   return 0;
}
