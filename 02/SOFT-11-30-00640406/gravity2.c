#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define WIDTH 100
#define HEIGHT 40

const double G = 1.0;  // gravity constant

typedef struct star
{
  double m;   // mass
  double x;   // position_x
  double vx;  // velocity_x
  double y;   // position_y
  double vy;  // velocity_y
} star;

star stars[] = {
  { 1.0, -30.0, 0.0, 10.0, 0.0},
  { 0.7,  -15.0, 0.2, -10, -0.1},
  { 0.8, -10.0, 0.3, 10.0, 0.0} 
};

const int nstars = sizeof(stars) / sizeof(star);

void plot_stars(FILE *fp, const double t)
{
  int i;
  char space[WIDTH][HEIGHT];

  memset(space, ' ', sizeof(space));
  for (i = 0; i < nstars; i++) {
    const int x = WIDTH  / 2 + stars[i].x;
    const int y = HEIGHT / 2 + stars[i].y;
    if (x < 0 || x >= WIDTH)  continue;
    if (y < 0 || y >= HEIGHT) continue;
    char c = 'o';
    if (stars[i].m >= 1.0){
      c= 'O';
    }else if (stars[i].m == 0){
      c = ' ';
    }
    space[x][y] = c;
  }

  int x, y;
  fprintf(fp, "----------\n");
  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++)
      fputc(space[x][y], fp);
    fputc('\n', fp);
  }
  fflush(fp);

  printf("t = %5.1f", t);
  for (i = 0; i < nstars; i++){ 
    printf(", stars[%d] = (%4.2f,%4.2f)", i, stars[i].x, stars[i].y);
  }
  printf("\n");
}

void update_velocities(const double dt)
{
  int i, j;
  for (i = 0; i < nstars; i++) {
    double ax = 0.0;
    double ay = 0.0;
    for (j = 0; j < nstars; j++){
      if (i == j){
        continue;
      }else{
        ax += G * stars[j].m * (stars[j].x - stars[i].x) \
              * pow( hypot(stars[j].x - stars[i].x, stars[j].y - stars[i].y) , -3);
        ay += G * stars[j].m * (stars[j].y - stars[i].y) \
              * pow( hypot(stars[j].x - stars[i].x, stars[j].y - stars[i].y) , -3); 
      }
    }
    stars[i].vx += ax * dt;
    stars[i].vy += ay * dt;
  }
}

void update_positions(const double dt) //衝突判定機能を追加
{
  int i,j;
  double M;
  float fusiondis = 1.0;
  for (i = 0; i < nstars; i++) {
    stars[i].x += stars[i].vx * dt;
    stars[i].y += stars[i].vy * dt;
  }
  for (i=0; i<nstars; i++){
    for (j=i+1; j<nstars; j++){
      if (hypot(stars[j].x - stars[i].x, stars[j].y - stars[i].y) < fusiondis){
        M = stars[i].m + stars[j].m;
        stars[i].vx = (stars[i].m*stars[i].vx + stars[j].m*stars[j].vx)/M;
        stars[i].vy = (stars[i].m*stars[i].vy + stars[j].m*stars[j].vy)/M;
        stars[i].m = M;
        stars[j].m = 0; 
      }
    }
  }
}

int main()
{
  const char *filename = "space.txt";
  FILE *fp;
  if ((fp = fopen(filename, "a")) == NULL) {
    fprintf(stderr, "error: cannot open %s.\n", filename);
    return 1;
  }

  const double dt = 0.5;
  const double stop_time = 400;

  int i;
  double t;
  for (i = 0, t = 0; t <= stop_time; i++, t += dt) {
    update_velocities(dt);
    update_positions(dt);
    if (i % 10 == 0) {
      plot_stars(fp, t);
      usleep(200 * 1000);
    }
  }

  fclose(fp);

  return 0;
}
