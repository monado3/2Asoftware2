#ifndef OPTIMIZE_OPTIMIZE_H
#define OPTIMIZE_OPTIMIZE_H

int optimize(const double alpha, const int dim, double x[], 
	     void (*calc_grad)(const double [], double []));

#endif
