#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct word{
   char* val;
   int frequency;
} word;

typedef struct Node{
   word new;
   struct node* next;
} Node;

int push_end( Node **head, word putit){
   Node *new_node = malloc(sizeof(Node));
   
   if(new_node != NULL){
      new_node->new = putit;
      new_node->next = 
   }


int main(int argc, char *argv[]){

   Node* listL[26] = {0};


}
