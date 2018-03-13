#include <stdio.h>

typedef struct student
{
  int id;
  char name[100];
  int age;
  double height;
  double weight;
} student;

typedef struct tagged_student1
{
  int id;
  char name[100];
  int age;
  double height;
  double weight;
  char tag;
} tagged_student1;

typedef struct tagged_student2
{
  char tag;
  int id;
  char name[100];
  int age;
  double height;
  double weight;
} tagged_student2;

int main (int argc, char**argv)
{
   student s;
   tagged_student1 s1;
   tagged_student2 s2;
   printf("studnet\n");
   printf("&id= %p\n",&(s.id));
   printf("&name= %p\n",s.name);
   printf("&age= %p\n",&(s.age));
   printf("&height= %p\n",&(s.height));
   printf("&weight= %p\n\n",&(s.weight));

   printf("tagged_student1\n");
   printf("&id= %p\n",&(s1.id));
   printf("&name= %p\n",s1.name);
   printf("&age= %p\n",&(s1.age));
   printf("&height= %p\n",&(s1.height));
   printf("&weight= %p\n",&(s1.weight));
   printf("&tag= %p\n\n",&(s1.tag));
   
   printf("tagged_student2\n");
   printf("&tag= %p\n",&(s2.tag));
   printf("&id= %p\n",&(s2.id));
   printf("&name= %p\n",s2.name);
   printf("&age= %p\n",&(s2.age));
   printf("&height= %p\n",&(s2.height));
   printf("&weight= %p\n",&(s2.weight));
   return 0;
}

/*
実行結果は
studnet
&id= 0x7fff53190a28
&name= 0x7fff53190a2c
&age= 0x7fff53190a90
&height= 0x7fff53190a98
&weight= 0x7fff53190aa0

tagged_student1
&id= 0x7fff531909a0
&name= 0x7fff531909a4
&age= 0x7fff53190a08
&height= 0x7fff53190a10
&weight= 0x7fff53190a18
&tag= 0x7fff53190a20

tagged_student2
&tag= 0x7fff53190920
&id= 0x7fff53190924
&name= 0x7fff53190928
&age= 0x7fff5319098c
&height= 0x7fff53190990
&weight= 0x7fff53190998
である。

studentではageはint型で4バイトのはずだが次のメンバ変数であるheightまで
8バイト確保されておりパティングが見られる。

tagged_student1でもageで同様のパティングが見られる。

tagged_student2ではtagはchar型で1バイトのはずだが次のメンバ変数であるidまで
4バイト確保されておりパティングが見られる。
*/