// list/list.c
// 
// Implementation for linked list.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

list_t *list_alloc() { 
  list_t* list = (list_t*)malloc(sizeof(list_t));
  list->head = NULL;
  return list; 
}

node_t *node_alloc(block_t *blk) {   
  node_t* node = malloc(sizeof(node_t));
  node->next = NULL;
  node->blk = blk;
  return node; 
}

void list_free(list_t *l){
  free(l);
  return;
}

void node_free(node_t *node){
  free(node);
  return;
}

void list_print(list_t *l) {
  node_t *activenode = l->head;
  block_t *b;
  
  if (activenode == NULL){
    printf("list is empty\n");
  }
  while (activenode != NULL){
    b = activenode->blk;
    printf("PID=%d START:%d END:%d", b->pid, b->start, b->end);
    activenode = activenode->next;
  }
}

int list_length(list_t *l) { 
  node_t *activenode = l->head;
  int counter = 0;
  while (activenode != NULL){
    counter++;
    activenode = activenode->next;
  }
  
  return counter; 
}

void list_add_to_back(list_t *l, block_t *blk){  
  node_t* newNode = node_alloc(blk);
  newNode->next = NULL;
  if(l->head == NULL){
    l->head = newNode;
  }
  else{
    node_t *activenode = l->head;
    while(activenode->next != NULL){
      activenode = activenode->next;
    }
    activenode->next = newNode;
  }
}

void list_add_to_front(list_t *l, block_t *blk){  
  node_t* newNode = node_alloc(blk);
 
  newNode->next = l->head;
  l->head = newNode;
}

void list_add_at_index(list_t *l, block_t *blk, int index){
  int counter = 0;
  
  node_t *newNode = node_alloc(blk);
  node_t *activenode = l->head;

  if(index == 0){
    newNode->next = l->head->next;
    l->head = newNode;
  }
  else if(index > 0){
    while(counter < index && activenode->next != NULL){
      activenode = activenode->next;
      counter++;
    }
  newNode->next = activenode->next;
  activenode->next = newNode;
  }
}

void list_add_ascending_by_address(list_t *l, block_t *newblk){

    node_t *activenode;
    node_t *previousnode;
    node_t *newNode = node_alloc(newblk);

    if(l->head == NULL){
      l->head = newNode;
    }
    else{
      previousnode = activenode = l->head;

      if(activenode->next == NULL) { 
         if(newblk->start <= activenode->blk->start) {  
            newNode->next = l->head;
            l->head = newNode;
         }
         else {   
            activenode->next = newNode;
            newNode->next = NULL;
         }
      }
      else {  

         if(newNode->blk->start <= activenode->blk->start) {  // 
            newNode->next = l->head;
            l->head = newNode;
         }
         else {
            while(activenode != NULL && newNode->blk->start >= activenode->blk->start) {
                 previousnode = activenode;
                 activenode = activenode->next;    
            }
            previousnode->next = newNode;
            newNode->next = activenode;
         }
      }
    }
}

void list_add_ascending_by_blocksize(list_t *l, block_t *newblk){

    node_t *activenode;
    node_t *previousnode;
    node_t *newNode = node_alloc(newblk);
    int newblk_size = newNode->blk->end - newNode->blk->start;
    int curr_blocksize;

    if(l->head == NULL){
      l->head = newNode;
    }
    else{
      previousnode = activenode = l->head;

      curr_blocksize = activenode->blk->end - activenode->blk->start + 1;

      if(activenode->next == NULL) { 
         if(newblk_size >= curr_blocksize) {  
            newNode->next = l->head;
            l->head = newNode;
         }
         else {   
            activenode->next = newNode;
            newNode->next = NULL;
         }
      }
      else {  
         if(newblk_size >= curr_blocksize) {  
            newNode->next = l->head;
            l->head = newNode;
         }
         else {

            while(activenode != NULL && newblk_size <= curr_blocksize) {
                 previousnode = activenode;
                 activenode = activenode->next;

                 if(activenode != NULL)  
                       curr_blocksize = activenode->blk->end - activenode->blk->start;
            }
            previousnode->next = newNode;
            newNode->next = activenode;
         }
      }
    }
}

void list_add_descending_by_blocksize(list_t *l, block_t *blk){
  node_t *activenode;
  node_t *previousnode;
  node_t *newNode = node_alloc(blk);
  int newblk_size = blk->end - blk->start;
  int curblk_size;
  
  if(l->head == NULL){
    l->head = newNode;
  }
  else{
    previousnode = activenode = l->head;
    
    curblk_size = activenode->blk->end - activenode->blk->start + 1;
    
    if(activenode->next == NULL) { 
       if(newblk_size >= curblk_size) {  
          newNode->next = l->head;
          l->head = newNode;
       }
       else {   
          activenode->next = newNode;
          newNode->next = NULL;
       }
    }
    else {  
      
       if(newblk_size >= curblk_size) {  
          newNode->next = l->head;
          l->head = newNode;
       }
       else {
      
          while(activenode != NULL && newblk_size <= curblk_size) {
               previousnode = activenode;
               activenode = activenode->next;
               
               if(activenode != NULL)  
                     curblk_size = activenode->blk->end - activenode->blk->start;
          }
          previousnode->next = newNode;
          newNode->next = activenode;
       }
    }
  }
}

void list_coalese_nodes(list_t *l){ 

    if (!l->head || !l->head->next){
      return;
    }

    node_t *previousnode = l->head;
    node_t *currentnode = l->head->next;

    while(currentnode){
      if (previousnode->blk->end + 1 == currentnode->blk->start){
        previousnode->blk->end = currentnode->blk->end;
        previousnode->next = currentnode->next;

        free(currentnode->blk);
        free(currentnode);

        currentnode = previousnode->next;
      }
      else{
        previousnode = currentnode;
        currentnode = currentnode->next;
      }
    }
}

block_t* list_remove_from_back(list_t *l){
  block_t *value = NULL;
  node_t *activenode = l->head;

  if(l->head != NULL){
    
    if(activenode->next == NULL) { 
         l->head->next = NULL;
         value = activenode->blk;
         node_free(activenode);
    }
    else {
         while (activenode->next->next != NULL){
            activenode = activenode->next;
         }
         value = activenode->blk;
         node_free(activenode->next);
         activenode->next = NULL;
    }
  }
  return value;
}

block_t* list_get_from_front(list_t *l) {
  block_t *value = NULL;
  if(l->head == NULL){
    return value;
  }
  else{
    node_t *activenode = l->head;
    value = activenode->blk;
  }
  return value; 
}


block_t* list_remove_from_front(list_t *l) { 
  block_t *value = NULL;
  if(l->head == NULL){
    return value;
  }
  else{
    node_t *activenode = l->head;
    value = activenode->blk;
    l->head = l->head->next;
    node_free(activenode);
  }
  return value; 
}

block_t* list_remove_at_index(list_t *l, int index) { 
  int counter;
  block_t* value = NULL;
  
  bool found = false;
  
  if(l->head == NULL){
    return value;
  }
  else if (index == 0){
    return list_remove_from_front(l);
  }
  else if (index > 0){
    node_t *activenode = l->head;
    node_t *previousnode = activenode;
    
    counter = 0;
    while(activenode != NULL && !found){
      if(counter == index)
          found = true;
      else {
         previousnode = activenode;
         activenode = activenode->next;
         counter++;
      }
    }
    
    if(found) {
      value = activenode->blk; 
      previousnode->next = activenode->next;
      node_free(activenode);
    }
  }
  return value; 
}

bool compareBlks(block_t* a, block_t *b) {
  
  if(a->pid == b->pid && a->start == b->start && a->end == b->end)
     return true;
  
  return false;
}

bool compareSize(int a, block_t *b) {  // greater or equal
  
  if(a <= (b->end - b->start) + 1)
     return true;
  
  return false;
}

bool comparePid(int a, block_t *b) {
  
  if(a == b->pid)
     return true;
  
  return false;
}


bool list_is_in(list_t *l, block_t* value) { 
  node_t *activenode = l->head;
  while(activenode != NULL){
    if(compareBlks(value, activenode->blk)){
      return true;
    }
    activenode = activenode->next;
  }
return false; 
}

block_t* list_get_elem_at(list_t *l, int index) { 
 int counter;
  block_t* value = NULL;
  if(l->head == NULL){
    return value;
  }
  else if (index == 0){
    return list_get_from_front(l);
  }
  else if (index > 0){
    node_t *activenode = l->head;
    
    counter = 0;
    while(activenode != NULL){
      if(counter == index)
          return(activenode->blk);
      else {
         activenode = activenode->next;
         counter++;
      }
    }
  }
  return value; 
}

int list_get_index_of(list_t *l, block_t* value){
 int counter = 0;
 node_t *activenode = l->head;
 if(l->head == NULL){
    return -1;
  }
  
  while (activenode != NULL){
   if (compareBlks(value,activenode->blk)){
     return counter;
    }
    activenode = activenode->next;
    counter++;
  }
  return -1; 
}

/* Checks to see if block of Size or greater exists in the list. */
bool list_is_in_by_size(list_t *l, int Size){ 
  node_t *activenode = l->head;
  while(activenode != NULL){
    if(compareSize(Size, activenode->blk)){
      return true;
    }
    activenode = activenode->next;
  }
return false; 
}

/* Checks to see if pid of block exists in the list. */
bool list_is_in_by_pid(list_t *l, int pid){ 
  

    node_t *activenode = l->head;
    while(activenode != NULL){
      if(comparePid(pid, activenode->blk)){
        return true;
      }
      activenode = activenode->next;
    }
    return false;
}

/* Returns the index at which the given block of Size or greater appears. */
int list_get_index_of_by_Size(list_t *l, int Size){
 int counter = 0;
 node_t *activenode = l->head;
 if(l->head == NULL){
    return -1;
  }
  
  while (activenode != NULL){
   if (compareSize(Size,activenode->blk)){
     return counter;
    }
    activenode = activenode->next;
    counter++;
  }

  return -1; 
}
                   
/* Returns the index at which the pid appears. */
int list_get_index_of_by_Pid(list_t *l, int pid){
 int counter = 0;
 node_t *activenode = l->head;
 if(l->head == NULL){
    return -1;
  }
  
  while (activenode != NULL){
   if (comparePid(pid,activenode->blk)){
     return counter;
    }
    activenode = activenode->next;
    counter++;
  }

  return -1; 
}