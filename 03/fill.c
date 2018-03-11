typedef struct {
   int sx;          /* 領域右端のx座標 */
   int sy;          /* 領域のy座標 */
} Seed;

Seed  seeds[WIDTH]; /* シード登録用配列 */
Seed *hidx, *eidx; /* seedsの先頭・末尾ポインタ */

/* int lx, rx : 線分のx座標の範囲
 * int y : 線分のy座標
 */
void scanline(int lx, int rx, int y)
{
   /* scanline :線分からシードを探索してseedsに登録する
    * int lx, rx : 線分のx座標の範囲
    * int y : 線分のy座標
    */
   while(lx <= rx) {

      //空白以外を飛ばす
      for(; lx <= rx; lx++) {
         if(canvas[lx, y] == ' ') {
            break;
         }
      }

      // すぐ前のfor文がbreakされなかったときのチェッカー
      if(canvas[lx, y] != ' ') {
         break;
      }

      //空白を飛ばす
      for(; lx <= rx; lx++) {
         if(canvas[lx, y] != ' ') {
            break;
         }
      }

      ﻿eidx->sx = lx - 1;
      eidx->sy  = y;
      if(++eidx == &seeds[WIDTH]) {//配列の最後の要素まで来てしまったとき
         eidx = seeds;
      }
   }
}

int fill(const int x0, const int y0, char localpen)
{   /* sで塗りつぶしたい領域内の輪郭以外のある点の座標を
     * 'fill x0 y0 s'のように入力する*/
   if(canvas[x0][y0] != ' ') {
      printf("That point isn't interior point of region.\n");
      return 1;
   }
   int lx, rx;                     /* 塗り潰す線分の両端のx座標 */
   int ly;                         /* 塗り潰す線分のy座標 */
   hidx     = seeds;
   eidx     = seeds + 1;
   hidx->sx = x0;
   hidx->sy = y0;

   do {
      lx = rx = hidx->sx;
      ly = hidx->sy;
      if(++hidx == &seeds[WIDTH]) {//配列の最後の要素まで来てしまったとき
         hidx = seeds;
      }

      /* 処理済のシードなら無視 */
      if(canvas[lx, ly] != ' ') {
         continue;
      }

      /* 右方向の境界を探す */
      while(rx < WIDTH - 1) {
         if(canvas[rx + 1, ly] != ' ') {
            break;
         }
         rx++;
      }
      /* 左方向の境界を探す */
      while(lx > 0) {
         if(canvas[lx - 1, ly] != ' ') {
            break;
         }
         lx--;
      }
      /* lx-rxの線分を描画 */
      draw_line(lx, ly, rx, ly);

      /* 真上のスキャンラインを走査する */
      if(ly - 1 >= 0) {
         scanline(lx, rx, ly - 1);
      }

      /* 真下のスキャンラインを走査する */
      if(ly + 1 <= HEIGHT - 1) {
         scanline(lx, rx, ly + 1);
      }
   } while(hidx != eidx);
}
