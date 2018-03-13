#ifndef TSP_PLOT_H
#define TSP_PLOT_H
void draw_line(const int x0, const int y0, const int x1, const int y1);
void draw_route(const int n, const int *route);
void plot_cities(FILE *fp, const int n, const int *route);

#endif
