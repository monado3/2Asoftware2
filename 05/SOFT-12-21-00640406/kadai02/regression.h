#ifndef OPTIMIZE2_REGRESSION_H
#define OPTIMIZE2_REGRESSION_H

double f_value(const double x[],const int nsamples, const int dim);
void f_gradient(const double x[], double g[], const int part[], const int nsamples, const int dim, const int startidx);

#endif
