#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <string.h>

#define NSYMBOLS (256 + 1)

#define MAXLEN   32

static int symbol_count[NSYMBOLS];

typedef struct node
{
   int  symbol;
   int  count;
   char code[MAXLEN];
   struct node* left;
   struct node* right;
} Node;


static void count_symbols(const char* filename)
{
   FILE* fp = fopen(filename, "rb");
   if(fp == NULL) {
      fprintf(stderr, "error: cannot open %s\n", filename);
      exit(1);
   }

   int i;
   for(i = 0; i < NSYMBOLS; i++) {
      symbol_count[i] = 0;
   }

   int chara;
   while((chara = fgetc(fp)) != EOF) {
      symbol_count[chara]++;
   }

   // symbol_count[NSYMBOLS - 1]++; // End of File

   fclose(fp);
}

static Node* pop_min(int* n, Node* nodep[])
{
   // Find the node with the smallest count
   int i, j = (*n) - 1;
   for(i = (*n) - 2; i >= 0; i--) {
      if(nodep[i]->count < nodep[j]->count) {
         j = i;
      }
   }
   Node* node_min = nodep[j];

   // Remove the node pointer from nodep[]
   for(i = j; i < (*n) - 1; i++) {
      nodep[i] = nodep[i + 1];
   }
   (*n)--;

   return node_min;
}

static Node* build_tree()
{
   int   i, n = 0;
   Node* nodep[NSYMBOLS];

   for(i = 0; i < NSYMBOLS; i++) {
      if(symbol_count[i] == 0) {
         continue;
      }
      nodep[n] = (Node*) malloc(sizeof(Node));
      nodep[n]->symbol = i;
      nodep[n]->count  = symbol_count[i];
      nodep[n]->left   = NULL;
      nodep[n]->right  = NULL;
      n++;
   }

   while(n >= 2) {
      Node* node1 = pop_min(&n, nodep);
      Node* node2 = pop_min(&n, nodep);

      // Create a new node
      nodep[n] = (Node*) malloc(sizeof(Node));
      nodep[n]->count = node1->count + node2->count;
      nodep[n]->left  = node2;
      nodep[n]->right = node1;
      n++;
   }

   return nodep[0];
}

// Perform depth-first traversal of the tree
static void traverse_tree(const int depth, Node* np)
{

   static char code[MAXLEN];

   assert(depth < NSYMBOLS);

   if(np->left == NULL) {
      code[depth] = '\0';
      strcpy(np->code, code);
      if(np->symbol != '\n') {
         printf(" %c (0x%02x) ( %5d times appeared ) : %11s\n", np->symbol, np->symbol, np->count,  np->code);
      } else {
         printf(" \\n(0x%02x) ( %5d times appeared ) : %11s\n", np->symbol, np->count, np->code);
      }

      return;
   }

   code[depth] = '0';
   traverse_tree(depth + 1, np->left);
   code[depth] = '1';
   traverse_tree(depth + 1, np->right);
}

int encode(const char* filename)
{
   count_symbols(filename);
   Node* root = build_tree();
   traverse_tree(0, root);

   return 1;
}
