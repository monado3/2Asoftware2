#ifndef OPTIMIZE_OPTIMIZE_H
#define OPTIMIZE_OPTIMIZE_H

int optimize(const double alpha, const int dim, const int nsamples, double x[],
             void (*calc_grad)(const double [], double [], const int  [], const int, const int, const int),
             double (*calc_val)(const double[], const int, const int));

#endif
