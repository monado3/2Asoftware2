#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#define WIDTH  200
#define HEIGHT 58

const double G = 1.0;   // gravity constant

typedef struct star {
   double m;            // mass
   double x;            // position_x
   double xc;           // position_x after dt/2 (middle point) (仮にvn*dt/2だけxnから進んだとしてx(n+0.5)の点を仮定します)
   double vx;           // velocity_x
   double y;            // position_y
   double yc;           // position_y after dt/2 (middle point) (xcと同様)
   double vy;           // velocity_y
} star;

star stars[1000] = {
   { 1.0, -30.0, 0.0, 0.0, 10.0, 0.0, 0.0  },
   { 0.7, -15.0, 0.0, 0.2, -10,  0.0, -0.0 },
   { 0.8, -10.0, 0.0, 0.3, 10.0, 0.0, 0.0  }
};

int nstars = 3;        // nstars includes a star whose mass is zero.

const int    bx = 0;   // x-coordinate of blackhole
const int    by = 0;   // y-coordinate of blackhole
const double bm = 30.0; // mass of blackhole
const int    BX = WIDTH / 2 + bx;
const int    BY = HEIGHT / 2 + by;


void plot_stars(FILE* fp, const double t)
{
   int  i;
   char space[WIDTH][HEIGHT];

   memset(space, ' ', sizeof(space));

   for(i = 0; i < nstars; i++) {
      if(stars[i].m == 0) continue;
      const int x = WIDTH  / 2 + stars[i].x;
      const int y = HEIGHT / 2 + stars[i].y;
      if(x < 0 || x >= WIDTH) continue;
      if(y < 0 || y >= HEIGHT) continue;
      char c = 'o';
      if(stars[i].m >= 1.0) c = 'O';
      space[x][y] = c;
   }
   space[BX][BY]     = 'B';
   space[BX - 1][BY] = 'B';
   space[BX + 1][BY] = 'B';
   space[BX][BY - 1] = 'B';
   space[BX][BY + 1] = 'B';


   int x, y;
   fprintf(fp, "----------\n");
   for(y = 0; y < HEIGHT; y++) {
      for(x = 0; x < WIDTH; x++) {
         fputc(space[x][y], fp);
      }
      fputc('\n', fp);
   }
   fflush(fp);

   printf("t = %5.1f\n", t);
}

void update_middlepoint(const double dt)    // 　dt/2後のx,yを求める。(中点法で加速度を計算するために設定する仮の点)
{
   int i;
   for(i = 0; i < nstars; i++) {
      if(stars[i].m == 0) continue;
      stars[i].xc = stars[i].x + 0.5 * stars[i].vx * dt;
      stars[i].yc = stars[i].y + 0.5 * stars[i].vy * dt;
   }
}

void update_velocities(const double dt) // 中点法を用いる
{
   int i, j;
   for(i = 0; i < nstars; i++) {
      if(stars[i].m == 0) continue;
      double ax = 0.0;
      double ay = 0.0;
      for(j = 0; j < nstars; j++) {
         if(i == j || stars[j].m == 0) {
            continue;
         } else {
            ax += G * stars[j].m * (stars[j].xc - stars[i].xc) *
                  pow(hypot(stars[j].xc - stars[i].xc, stars[j].yc - stars[i].yc), -3);
            ay += G * stars[j].m * (stars[j].y - stars[i].y) *
                  pow(hypot(stars[j].xc - stars[i].xc, stars[j].yc - stars[i].yc), -3);
         }
      }
      ax += G * bm * (bx - stars[i].xc) *
            pow(hypot(bx - stars[i].xc, bx - stars[i].yc), -3);
      ay += G * bm * (by - stars[i].y) *
            pow(hypot(by - stars[i].xc, by - stars[i].yc), -3);
      stars[i].vx += ax * dt;
      stars[i].vy += ay * dt;
   }
}

void update_positions(const double dt)  // 衝突判定機能・ブラックホールへの吸引を追加
{
   int    i, j;
   double M;
   float  mergedis   = 1.0;// 星同士の合体判定距離
   float  accretedis = 1.0;// ブラックホールへの吸引判定
   for(i = 0; i < nstars; i++) {
      stars[i].x += stars[i].vx * dt;
      stars[i].y += stars[i].vy * dt;
   }
   for(i = 0; i < nstars; i++) {
      if( hypot(stars[i].x - bx, stars[j].y - by) < accretedis) {
         stars[i].m = 0;continue;
      }
         for(j = i + 1; j < nstars; j++) {
            if(hypot(stars[j].x - stars[i].x, stars[j].y - stars[i].y) < mergedis) {
               M = stars[i].m + stars[j].m;
               stars[i].vx = (stars[i].m * stars[i].vx + stars[j].m * stars[j].vx) / M;
               stars[i].vy = (stars[i].m * stars[i].vy + stars[j].m * stars[j].vy) / M;
               stars[i].m  = M;
               stars[j].m  = 0;
         }
      }
   }
}

void generate_star(void) // 星の生成
{
   stars[nstars].m  = (rand() % 150) / 100.0;
   stars[nstars].x  = (rand() % (10*2*WIDTH/3)) / 10.0 - (WIDTH / 3.0);
   stars[nstars].y  = (rand() % (10*2*HEIGHT/3)) / 10.0 - (HEIGHT / 3.0);
   stars[nstars].vx = (rand() % 150) / 100.0 - 0.75;
   stars[nstars].vy = (rand() % 150) / 100.0 - 0.75;
   printf("m=%f\nx=%f y=%f\nvx=%f vy=%f\n",stars[nstars].m,stars[nstars].x,stars[nstars].y,stars[nstars].vx,stars[nstars].vy);
   nstars++;
}

int main(int argc, char* argv[])
{
   /* ./gravity3 星の生成周期
    * のように生成周期は変更できます。デフォルトでは10.0です。*/

   const char* filename = "space.txt";
   FILE*       fp;
   if((fp = fopen(filename, "a")) == NULL) {
      fprintf(stderr, "error: cannot open %s.\n", filename);
      return 1;
   }

   const double stop_time = 100;
   const double dt = 0.1;
   double       period; // 星の生成周期
   if(argc == 2) {
      period = atof(argv[1]);
   } else {
      period = 10.0;
   }

   srand(time(NULL));

   int    i;
   double t;
   int period_i=period/dt; 
   for(i = 0, t = 0; t <= stop_time; i++, t += dt) {
      update_middlepoint(dt);
      update_velocities(dt);
      update_positions(dt);
      if( i % period_i == 0) {
         generate_star();
      }
      if(i % 10 == 0) {
         plot_stars(fp, t);
         usleep(200 * 1000);
      }
   }

   fclose(fp);

   return 0;
}

/*追加した機能
 * 衝突判定機能
 * オイラー法から中点法に変更し精度の向上
 * ブラックホールの追加（星の吸収、ブラックホールは不動）
 * 星の一定周期ごとの生成
 * コマンドライン引数からの生成周期の変更
 * 標準出力へ出力される情報の変更
 */
