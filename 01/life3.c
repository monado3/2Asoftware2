#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define HEIGHT 40
#define WIDTH 70

int cell[HEIGHT][WIDTH];

void init_cells(FILE *fp)
{
    int i, j,c;
    fseek(fp,-1*(HEIGHT*(WIDTH+1))+1,SEEK_END);
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j <= WIDTH; j++) {
            if ( ( c = getc(fp) ) == '\n' ){
                continue;
            }else if ( c=='#'){
                cell[i][j]=1;
            }else{
                cell[i][j]=0;
            }
        }
    }
}

void print_cells(FILE *fp)
{
    int i, j;

    fprintf(fp, "----------\n");

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            const char c = (cell[i][j] == 1) ? '#' : ' ';
            fputc(c, fp);
        }
        fputc('\n', fp);
    }
    fflush(fp);

    sleep(1);
}

double seek_cellratio(){
    int totalcell=HEIGHT*WIDTH;
    int i, j,counter=0;
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (cell[i][j]==1) {
                counter++;
            }
        }
    }
    return (double)counter/totalcell;
}

int count_adjacent_cells(int i, int j)
{
    int n = 0;
    int k, l;
    for (k = i - 1; k <= i + 1; k++) {
        if (k < 0 || k >= HEIGHT) continue;
        for (l = j - 1; l <= j + 1; l++) {
            if (k == i && l == j) continue;
            if (l < 0 || l >= WIDTH) continue;
            n += cell[k][l];
        }
    }
    return n;
}

void update_cells()
{
    int i, j;
    int cell_next[HEIGHT][WIDTH];

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            cell_next[i][j] = 0;
            int n = count_adjacent_cells(i, j);
            if (cell[i][j]==1) {
                if (n==2 || n==3) {
                    cell_next[i][j]=1;
                }
            }else {
                if (n==3) {
                    cell_next[i][j]=1;
                }
            }
        }
    }

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            cell[i][j] = cell_next[i][j];
        }
    }
}


int main()
{
    int gen;
    FILE *fp;
  
    if ((fp = fopen("cells.txt", "r")) == NULL) {
        fprintf(stderr, "error: cannot open a file.\n");
        return 1;
    }
    init_cells(fp);
    fclose(fp);

    if ((fp = fopen("cells.txt", "a")) == NULL) {
        fprintf(stderr, "error: cannot open a file.\n");
        return 1;
    }
    
    print_cells(fp);

    for (gen = 1;; gen++) {
        printf("generation = %d\nlivelycell/total = %lf\n", gen,seek_cellratio());
        update_cells();
        print_cells(fp);
    }

    fclose(fp);

    return 0;
}
