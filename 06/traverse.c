#include <stdio.h>

typedef struct node
{
   int value;
   struct node* left;
   struct node* right;
} Node;

void traverse(const Node* n)
{
   if(n == NULL) {
      return;
   }

   printf("value = %d\n", n->value);

   traverse(n->left);
   traverse(n->right);

}

int main()
{
   Node n1  = {  1, NULL, NULL };
   Node n2  = {  2, NULL, NULL };
   Node n3  = {  3, NULL, NULL };
   Node n4  = {  4, NULL, NULL };
   Node n5  = {  5, NULL, NULL };
   Node n6  = {  6, NULL, NULL };
   Node n7  = {  7, NULL, NULL };
   Node n8  = {  8, NULL, NULL };
   Node n9  = {  9, NULL, NULL };
   Node n10 = { 10, NULL, NULL };

   n1.left  = &n2;
   n1.right = &n7;
   n2.left  = &n3;
   n2.right = &n6;
   n3.left  = &n4;
   n3.right = &n5;
   n7.left  = &n8;
   n7.right = &n10;
   n8.left  = &n9;


   traverse(&n1);
}
