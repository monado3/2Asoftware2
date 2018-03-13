#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

#define WIDTH       70
#define HEIGHT      40

#define MAX_HISTORY 100
#define BUFSIZE     1000
#define MAXLEN      1000

char canvas[WIDTH][HEIGHT];
const char* default_history_file = "history.txt";

// 以下線形リストの実装//
typedef struct node {
   char* str;
   struct node* next;
} Node;

Node* begin = NULL; // 線形リストの先頭要素ポインタをグローバル宣言
int interpret_command(const char*, int*);


Node* push_front(Node* localbegin, const char* str)
{
   // Create a new element
   Node* p = (Node*) malloc(sizeof(Node));
   char* s = (char*) malloc(strlen(str) + 1);
   strcpy(s, str);
   p->str  = s;
   p->next = localbegin;

   return p; // Now the new element is the first element in the list
}

Node* pop_front(Node* localbegin)
{
   if(localbegin == NULL) {
      printf("The command history is empty.\n");
      return localbegin;
   }  // Don't call pop_front() when the list is empty
   Node* p = localbegin->next;

   free(localbegin->str);
   free(localbegin);

   return p;
}

Node* push_back(Node* localbegin, const char* str)
{
   if(localbegin == NULL) {   // If the list is empty
      return push_front(localbegin, str);
   }

   // Find the last element
   Node* p = localbegin;
   while(p->next != NULL) {
      p = p->next;
   }

   // Create a new element
   Node* q = (Node*) malloc(sizeof(Node));
   char* s = (char*) malloc(strlen(str) + 1);
   strcpy(s, str);
   q->str  = s;
   q->next = NULL;

   // The new element should be linked from the previous last element
   p->next = q;

   return localbegin;
}

Node* remove_all(Node* localbegin)
{
   while((localbegin = pop_front(localbegin))) {
      // Repeat pop_front() until the list becomes empty
   }
   return localbegin; // Now, begin is NULL
}

Node* pop_back(Node* localbegin)
{
   if(localbegin == NULL) { // If the list is empty.
      printf("The command history is empty.\n");
      return localbegin;
   }

   if(localbegin->next == NULL) { // If the list has one element.
      return pop_front(localbegin);
   }

   Node* p = localbegin;
   while(p->next->next != NULL) {
      p = p->next;
   }
   free(p->next->next);
   p->next = NULL;

   return localbegin;
}

// 以上線形リストの実装

void print_canvas(FILE* fp)
{
   int x, y;

   fprintf(fp, "----------\n");

   for(y = 0; y < HEIGHT; y++) {
      for(x = 0; x < WIDTH; x++) {
         const char c = canvas[x][y];
         fputc(c, fp);
      }
      fputc('\n', fp);
   }
   fflush(fp);
}

void init_canvas()
{
   memset(canvas, ' ', sizeof(canvas));
}

int max(const int a, const int b)
{
   return (a > b) ? a : b;
}

void draw_line(const int x0, const int y0, const int x1, const int y1)
{
   int       i;
   const int n = max(abs(x1 - x0), abs(y1 - y0));
   if(n == 0) {
      canvas[x0][y0] = '#';
   } else {
      for(i = 0; i <= n; i++) {
         const int x = x0 + i * (x1 - x0) / n;
         const int y = y0 + i * (y1 - y0) / n;
         canvas[x][y] = '#';
      }
   }
}

int draw_rectangle(const int ax, const int ay, const int bx, const int by)
{  // 対頂点a,bの座標を 'rec ax ay bx by'のように入力する
   if(ax == bx || ay == by) {
      printf("these coordinates don't draw a rectangle.\n");
      return 1;
   }
   draw_line(ax, ay, ax, by);
   draw_line(ax, ay, bx, ay);
   draw_line(ax, by, bx, by);
   draw_line(bx, ay, bx, by);

   return 0;
}

int draw_circle(const int x0, const int y0, const double r)
{  // 中心座標(x0,y0)と円の半径を'cir x0 y0 r'のように入力する
   if(r <= 0) {
      printf("The radius needs to be positive.\n");
      return 1;
   }

   double i;
   for(i = 0.01; i <= 2 * M_PI; i += 0.1) {
      const int x = x0 + r * cos(i);
      const int y = y0 + r * sin(i);
      canvas[x][y] = '#';
   }

   return 0;
}

void save_history(const char* filename, const int history_size)
{
   if(filename == NULL) {
      filename = default_history_file;
   }

   FILE* fp;
   if((fp = fopen(filename, "w")) == NULL) {
      fprintf(stderr, "error: cannot open %s.\n", filename);
      return;
   }

   int   i;
   Node* p = begin;
   for(i = 0; i < history_size; i++, p = p->next) {
      fprintf(fp, "%s", p->str);
   }

   printf("saved as \"%s\"\n", filename);

   fclose(fp);
}

int load_history(const char* filename)
{
   if(filename == NULL) {
      filename = default_history_file;
   }

   FILE* fp;
   if((fp = fopen(filename, "r")) == NULL) {
      fprintf(stderr, "error: cannot open %s.\n", filename);
      return 1;
   }

   init_canvas();

   int i = 0;
   begin = NULL;
   char buf[MAXLEN];
   while(fgets(buf, MAXLEN, fp)) {
      interpret_command(buf, NULL);
      begin = push_back(begin, buf);
      i++;
   }
   printf("loaded from \"%s\"\n", filename);

   fclose(fp);
   return i;
}

// Interpret and execute a command
//   return value:
//     0, normal commands such as "line"
//     1, unknown or special commands (not recorded in history[])
//     2, quit
int interpret_command(const char* command, int* hsize)
{
   int  i;
   char buf[BUFSIZE];
   strcpy(buf, command);
   buf[strlen(buf) - 1] = 0; // remove the newline character at the end

   const char* s = strtok(buf, " ");

   if(strcmp(s, "line") == 0) {
      int x0, y0, x1, y1;
      x0 = atoi(strtok(NULL, " "));
      y0 = atoi(strtok(NULL, " "));
      x1 = atoi(strtok(NULL, " "));
      y1 = atoi(strtok(NULL, " "));
      draw_line(x0, y0, x1, y1);
      return 0;
   }

   if(strcmp(s, "rec") == 0) {
      int ax, ay, bx, by, check;
      ax    = atoi(strtok(NULL, " "));
      ay    = atoi(strtok(NULL, " "));
      bx    = atoi(strtok(NULL, " "));
      by    = atoi(strtok(NULL, " "));
      check = draw_rectangle(ax, ay, bx, by);
      return check;
   }

   if(strcmp(s, "cir") == 0) {
      int    x0, y0, check;
      double r;
      x0    = atoi(strtok(NULL, " "));
      y0    = atoi(strtok(NULL, " "));
      r     = atof(strtok(NULL, " "));
      check = draw_circle(x0, y0, r);
      return check;
   }

   if(strcmp(s, "save") == 0) {
      s = strtok(NULL, " ");
      save_history(s, *hsize);
      return 1;
   }

   if(strcmp(s, "load") == 0) {
      s      = strtok(NULL, " ");
      *hsize = load_history(s);
      return 1;
   }

   if(strcmp(s, "undo") == 0) {
      init_canvas();
      Node* p = begin;
      for(i = 0; i < *hsize - 1; i++, p = p->next) {
         interpret_command(p->str, NULL);
      }
      begin = pop_back(begin);
      if(*hsize > 0) {
         (*hsize)--;
      }
      return 1;
   }

   if(strcmp(s, "quit") == 0) {
      return 2;
   }

   printf("error: unknown command.\n");

   return 1;
}

int main()
{
   const char* filename = "canvas.txt";
   FILE*       fp;
   char buf[BUFSIZE];

   if((fp = fopen(filename, "a")) == NULL) {
      fprintf(stderr, "error: cannot open %s.\n", filename);
      return 1;
   }

   init_canvas();
   print_canvas(fp);

   int hsize = 0; // history size
   while(1) {
      printf("%d > ", hsize);
      fgets(buf, BUFSIZE, stdin);

      const int r = interpret_command(buf, &hsize);
      if(r == 2) {
         break;
      }
      if(r == 0) {
         begin = push_back(begin, buf);
         hsize++;
      }

      print_canvas(fp);
   }

   fclose(fp);

   return 0;
}
