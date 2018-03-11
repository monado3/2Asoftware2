#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct student
{
  int id;
  char name[100];
} Student;

int comp_name(const void* x0, const void *x1)
{
  const Student *p0 = (Student*)x0;
  const Student *p1 = (Student*)x1;

  // Comparison part for type Student
  // p0->name and p1->name should be evaluated

  // return ????????
  return strcmp(p0->name,p1->name);
}


int main(int argc, char**argv)
{
  Student v[] = {13, "situ", 4, "ozk", 9, "ire", 15, "tnmr", 7, "fjmt"};
  const int n = sizeof(v) / sizeof(v[0]);

  // before sort
  printf("== before ==\n");
  for (int i = 0 ; i < n ; i++)
    {
      printf("v[%d] = %s\n",i, v[i].name);
    }
  qsort(v,n,sizeof(v[0]),comp_name);

  // after qsort
  printf("== after ==\n");
  for (int i = 0 ; i < n ; i++)
    {
      printf("v[%d] = %s\n",i, v[i].name);
    }
  return 0;

}
