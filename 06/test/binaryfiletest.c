#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
   char filename[]="test.txt";
   FILE* fp = fopen(filename, "rb");
   if(fp == NULL) {
      fprintf(stderr, "error: cannot open %s\n", filename);
      exit(1);
   }

   int s[257];
   fgets(s , 256, fp);
   printf("%c\n", s);

   return 0;
}
