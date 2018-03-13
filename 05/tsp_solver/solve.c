#include <stdio.h>
#include "city.h"
#include "solve.h"
#include <math.h>

static double distance(const int i, const int j)
{
  const double dx = city[i].x - city[j].x;
  const double dy = city[i].y - city[j].y;
  return sqrt(dx * dx + dy * dy);
}

double solve(const int n, int route[])
{
  int i;

  route[0] = 0;  // Start from city[0]

  // This is dummy. Replace this with your own code to compute a solution.
  for (i = 1; i < n; i++) {
    route[i] = i;
  }
  

  // Compute the total distance
  double sum_d = 0;
  for (i = 0; i < n; i++) {
    const int c0 = route[i];
    const int c1 = route[(i+1)%n];
    sum_d += distance(c0, c1);
  }
  return sum_d;
}
