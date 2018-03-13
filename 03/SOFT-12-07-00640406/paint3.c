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

char gcanvas[WIDTH][HEIGHT];
const char* default_history_file = "history.txt";
char gpen = '#';

// 以下線形リストの実装//
typedef struct node {
   char* str;
   struct node* next;
} Node;

Node* gbegin = NULL; // 線形リストの先頭要素ポインタをグローバル宣言
int   interpret_command(const char*, int*);
void  draw_line(const int, const int, const int, const int);

Node* push_front(Node* begin, const char* str)
{
   // Create a new element
   Node* p = (Node*) malloc(sizeof(Node));
   char* s = (char*) malloc(strlen(str) + 1);
   strcpy(s, str);
   p->str  = s;
   p->next = begin;

   return p; // Now the new element is the first element in the list
}

Node* pop_front(Node* begin)
{
   if(begin == NULL) {
      printf("The command history is empty.\n");
      return begin;
   }  // Don't call pop_front() when the list is empty
   Node* p = begin->next;

   free(begin->str);
   free(begin);

   return p;
}

Node* push_back(Node* begin, const char* str)
{
   if(begin == NULL) {   // If the list is empty
      return push_front(begin, str);
   }

   // Find the last element
   Node* p = begin;
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

   return begin;
}

Node* remove_all(Node* begin)
{
   while((begin = pop_front(begin))) {
      // Repeat pop_front() until the list becomes empty
   }
   return begin; // Now, gbegin is NULL
}

Node* pop_back(Node* begin)
{
   if(begin == NULL) { // If the list is empty.
      printf("The command history is empty.\n");
      return begin;
   }

   if(begin->next == NULL) { // If the list has one element.
      return pop_front(begin);
   }

   Node* p = begin;
   while(p->next->next != NULL) {
      p = p->next;
   }
   free(p->next->next);
   p->next = NULL;

   return begin;
}

// 以上線形リストの実装

// 以下Scanline Seed Fill Algorithmの実装
typedef struct {
   int sx;            // 領域右端のx座標
   int sy;            // 領域のy座標
} Seed;

Seed  gseeds[WIDTH];  // シード登録用配列
Seed* ghidx, * geidx; // gseedsの先頭・末尾ポインタ

// int lx, rx : 線分のx座標の範囲
// int y : 線分のy座標

void scanline(int lx, int rx, int y)
{
   /* scanline :線分からシードを探索してgseedsに登録する
    * int lx, rx : 線分のx座標の範囲
    * int y : 線分のy座標
    */
   while(lx <= rx) {
      // 空白以外を飛ばす
      for(; lx <= rx; lx++) {
         if(gcanvas[lx][y] == ' ') {
            break;
         }
      }

      // すぐ前のfor文がbreakされなかったときのチェッカー
      if(gcanvas[lx][y] != ' ') {
         break;
      }

      // 空白を飛ばす
      for(; lx <= rx; lx++) {
         if(gcanvas[lx][y] != ' ') {
            break;
         }
      }

      geidx->sx = lx - 1;
      geidx->sy = y;
      if(++geidx == &gseeds[WIDTH]) { // 配列の最後の要素まで来てしまったとき
         geidx = gseeds;
      }
   }
}

int fill(const int x0, const int y0)
{   /* sで塗りつぶしたい領域内の輪郭以外のある点の座標を
     * 'fill x0 y0 s'のように入力する*/
   if(gcanvas[x0][y0] != ' ') {
      printf("That point isn't interior point of region.\n");
      return 1;
   }

   int lx, rx; // 塗り潰す線分の両端のx座標
   int ly;     // 塗り潰す線分のy座標
   ghidx     = gseeds;
   geidx     = gseeds + 1;
   ghidx->sx = x0;
   ghidx->sy = y0;

   do {
      lx = rx = ghidx->sx;
      ly = ghidx->sy;
      if(++ghidx == &gseeds[WIDTH]) { // 配列の最後の要素まで来てしまったとき
         ghidx = gseeds;
      }

      // 処理済のシードなら無視
      if(gcanvas[lx][ly] != ' ') {
         continue;
      }

      // 右方向の境界を探す
      while(rx < WIDTH - 1) {
         if(gcanvas[rx + 1][ly] != ' ') {
            break;
         }
         rx++;
      }
      // 左方向の境界を探す
      while(lx > 0) {
         if(gcanvas[lx - 1][ly] != ' ') {
            break;
         }
         lx--;
      }
      // lx-rxの線分を描画 //
      draw_line(lx, ly, rx, ly);

      // 真上のスキャンラインを走査する
      if(ly - 1 >= 0) {
         scanline(lx, rx, ly - 1);
      }

      // 真下のスキャンラインを走査する
      if(ly + 1 <= HEIGHT - 1) {
         scanline(lx, rx, ly + 1);
      }
   } while(ghidx != geidx);

   return 0;
}

// 以上Scanline Seed Fill Algorithmの実装

void print_canvas(FILE* fp) // 座標の表示
{
   int x, y;

   fprintf(fp, "  ");
   for(x = 0; x < WIDTH; x++) {
      if(x == 0 || x == 5) {
         fprintf(fp, "%d ", x);
      } else if(x % 5 == 0 || x == WIDTH - 1) {
         fprintf(fp, "%d", x);
      } else if(x % 5 == 1) {
      } else {
         fputc(' ', fp);
      }
   }
   fputc('\n', fp);
   for(y = 0; y < HEIGHT; y++) {
      if(y == 0 || y == 5) {
         fprintf(fp, "%d ", y);
      } else if(y % 5 == 0 || y == HEIGHT - 1) {
         fprintf(fp, "%d", y);
      } else {
         fprintf(fp, "  ");
      }
      for(x = 0; x < WIDTH; x++) {
         const char c = gcanvas[x][y];
         fputc(c, fp);
      }
      fputc('\n', fp);
   }
   fflush(fp);
}

void init_canvas()
{
   memset(gcanvas, ' ', sizeof(gcanvas));
}

int max(const int a, const int b)
{
   return (a > b) ? a : b;
}

void draw_dot(const int x0, const int y0)
{  // 座標を'dot x0 y0'のように入力する
   gcanvas[x0][y0] = gpen;
}

void draw_line(const int x0, const int y0, const int x1, const int y1)
{
   int       i;
   const int n = max(abs(x1 - x0), abs(y1 - y0));
   if(n == 0) {
      gcanvas[x0][y0] = gpen;
   } else {
      for(i = 0; i <= n; i++) {
         const int x = x0 + i * (x1 - x0) / n;
         const int y = y0 + i * (y1 - y0) / n;
         gcanvas[x][y] = gpen;
      }
   }
}

int draw_rectangle(const int ax, const int ay, const int bx, const int by)
{  // 対頂点a,bの座標を 'rec ax ay bx by'のように入力する
   // 座標に並行でない長方形はどうせ崩れてしまうので並行な物のみ描画する仕様にした
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
      gcanvas[x][y] = gpen;
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
   Node* p = gbegin;
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
   gbegin = NULL;
   int  hsizecounter = 0;
   char buf[BUFSIZE];
   while(fgets(buf, BUFSIZE, fp)) {
      interpret_command(buf, NULL);
      gbegin = push_back(gbegin, buf);
      hsizecounter++;
   }
   printf("loaded from \"%s\"\n", filename);

   fclose(fp);
   return hsizecounter;
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
   if(strlen(buf) == 1) {      // コマンドを標準入力に何も入力せずにEnterを押したときに
      return 1;                // 落ちる現象を修正
   }
   buf[strlen(buf) - 1] = 0;   // remove the newline character at the end
   static int savechecker = 1; // quitコマンド時に直前にsave or loadしてないか判定用
   static int cnt = 0;         // quitコマンド時に直前にsave or loadしてないか判定用

   char* s = strtok(buf, " ");
   if(savechecker == 1 && cnt == 0) {
      cnt++;
   } else {
      savechecker = 0;         // リセット
      cnt = 0;                 // リセット
   }

   if(strcmp(s, "dot") == 0) { // 点を描画,座標を'dot x0 y0'のように入力する
      int x0, y0;
      x0 = atoi(strtok(NULL, " "));
      y0 = atoi(strtok(NULL, " "));
      draw_dot(x0, y0);
      return 0;
   }

   if(strcmp(s, "line") == 0) {
      int x0, y0, x1, y1;
      x0 = atoi(strtok(NULL, " "));
      y0 = atoi(strtok(NULL, " "));
      x1 = atoi(strtok(NULL, " "));
      y1 = atoi(strtok(NULL, " "));
      draw_line(x0, y0, x1, y1);
      return 0;
   }

   if(strcmp(s, "rec") == 0) {   // 長方形を描画,対頂点a,bの座標を
      int ax, ay, bx, by, check; // 'rec ax ay bx by'のように入力する
      ax    = atoi(strtok(NULL, " "));
      ay    = atoi(strtok(NULL, " "));
      bx    = atoi(strtok(NULL, " "));
      by    = atoi(strtok(NULL, " "));
      check = draw_rectangle(ax, ay, bx, by);
      if(check == 1) {
         cnt = 0;
      }
      return check;
   }

   if(strcmp(s, "cir") == 0) { // 円を描画,中心座標(x0,y0)と円の半径を
      int    x0, y0, check;    //  'cir x0 y0 r'のように入力する
      double r;
      x0    = atoi(strtok(NULL, " "));
      y0    = atoi(strtok(NULL, " "));
      r     = atof(strtok(NULL, " "));
      check = draw_circle(x0, y0, r);
      if(check == 1) {
         cnt = 0;
      }
      return check;
   }

   if(strcmp(s, "fill") == 0) { // 塗りつぶし 塗りつぶす領域内の内点の座標を
      int x0, y0, check;        // 'fill x0 y0'のように入力する
      x0    = atoi(strtok(NULL, " "));
      y0    = atoi(strtok(NULL, " "));
      check = fill(x0, y0);
      if(check == 1) {
         cnt--;
      }
      return check;
   }


   if(strcmp(s, "pen") == 0) { // 描画する文字を変更 デフォルトは#
      s = strtok(NULL, " ");   // %に変更したいとき'pen %'のように入力する
      if(s == NULL) {
         gpen = '#';
      } else {
         gpen = *s;
      }
      return 0;
   }

   if(strcmp(s, "erase") == 0) { // 消しゴムモード 解除はpenコマンド
      gpen = ' ';
      return 0;
   }

   if(strcmp(s, "save") == 0) {
      s = strtok(NULL, " ");
      save_history(s, *hsize);
      savechecker = 1;
      return 1;
   }

   if(strcmp(s, "load") == 0) { // loadする際変更を保存しなくていいか質問
      s = strtok(NULL, " ");
      if(savechecker == 1) {
         gpen = '#';
         *hsize      = load_history(s);
         savechecker = 1;
         return 1;
      } else {
         printf("The commandhistory has changes, do you want to save them?\n"
                "y(save) or c(cancel loading) or n(don't save)\n");
         fgets(buf, BUFSIZE, stdin);
         if(*buf == 'n') {
            gpen = '#';
            *hsize      = load_history(s);
            savechecker = 1;
            return 1;
         } else if(*buf == 'y') {
            printf("Please enter the destination for the filename.\n"
                   "If you just press the Enter key,"
                   " the destination is %s.\n", default_history_file);
            char buf1[BUFSIZE];
            fgets(buf1, BUFSIZE, stdin);
            if (strlen(buf1) > 1) {
               buf1[strlen(buf1) - 1] = '\0';  
            } else {
               save_history(NULL, *hsize);
               gpen = '#';
               *hsize      = load_history(s);
               savechecker = 1;
               return 1;
            }
            save_history(buf1, *hsize);
            gpen = '#';
            *hsize      = load_history(s);
            savechecker = 1;
            return 1;
         } else {
            return 1;
         }
      }
      s    = strtok(NULL, " ");
      gpen = '#';
      *hsize      = load_history(s);
      savechecker = 1;
      return 1;
   }

   if(strcmp(s, "undo") == 0) {
      init_canvas();
      Node* p = gbegin;
      gpen = '#';
      for(i = 0; i < *hsize - 1; i++, p = p->next) {
         interpret_command(p->str, NULL);
      }
      gbegin = pop_back(gbegin);
      if(*hsize > 0) {
         (*hsize)--;
      }
      return 1;
   }

   if(strcmp(s, "init") == 0) {                // キャンバスと履歴を真っ白にする
      printf("Do you really initialize the canvas"
             " and commandhistory? y or n\n"); // 本当に初期化してよいかの確認
      fgets(buf, BUFSIZE, stdin);
      if(*buf == 'y') {
         init_canvas();
         gbegin = remove_all(gbegin);
         *hsize = 0;
         gpen   = '#';
      }
      cnt = 0;
      return 1;
   }


   if(strcmp(s, "quit") == 0) { // 大幅にアップグレード
      if(savechecker == 1) {
         return 2;
      } else {
         printf("The commandhistory has changes, do you want to save them?\n"
                "y(save) or c(cancel quitting) or n(don't save)\n");
         fgets(buf, BUFSIZE, stdin);
         if(*buf == 'n') {
            return 2;
         } else if(*buf == 'y') {
            printf("Please enter the destination for the filename.\n"
                   "If you just press the Enter key,"
                   " the destination is %s.\n", default_history_file);
            fgets(buf, BUFSIZE, stdin);
            if (strlen(buf) > 1) {
               buf[strlen(buf) - 1] = '\0';  
            } else {
               save_history(NULL, *hsize);
               return 2;
            }
            save_history(buf, *hsize);
            return 2;
         } else {
            return 1;
         }
      }
   }

   printf("error: unknown command.\n");
   cnt = 0;

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
         gbegin = push_back(gbegin, buf);
         hsize++;
      }
      print_canvas(fp);
   }

   fclose(fp);

   return 0;
}

/*変更点
 * lineコマンドで一点を指すと落ちる現象を修正
 * quitコマンド時にsaveしていなければ確認する機能を追加
 * コマンドを標準入力に何も入力せずにEnterを押したときに落ちる現象を修正
 * print_canvasを変更し座標の表示
 * loadコマンドでsaveしてから保存するかの確認機能を追加
 *
 *追加したコマンド
 * dot 点の描画
 * fill 塗りつぶし(Scanline Seed Fill Algorithmを用いた)
 * pen 描画する時の文字を変更（この変更によりいくつかの関数(undo等)に
 * デフォルトが#であることに起因する調整あり)
 * erase 消しゴムモード
 * init キャンバスと履歴の初期化(本当に初期化していいのかの確認あり)
 */
