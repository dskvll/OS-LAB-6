#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "list.h"
#include "util.h"

void TOUPPER(char * arr){
  
    for(int i=0;i<strlen(arr);i++){
        arr[i] = toupper(arr[i]);
    }
}

void get_input(char *args[], int input[][2], int *n, int *size, int *policy) 
{
  	FILE *input_file = fopen(args[1], "r");
	  if (!input_file) {
		    fprintf(stderr, "Error: Invalid filepath\n");
		    fflush(stdout);
		    exit(0);
	  }

    parse_file(input_file, input, n, size);
  
    fclose(input_file);
  
    TOUPPER(args[2]);
  
    if((strcmp(args[2],"-F") == 0) || (strcmp(args[2],"-FIFO") == 0))
        *policy = 1;
    else if((strcmp(args[2],"-B") == 0) || (strcmp(args[2],"-BESTFIT") == 0))
        *policy = 2;
    else if((strcmp(args[2],"-W") == 0) || (strcmp(args[2],"-WORSTFIT") == 0))
        *policy = 3;
    else {
       printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
       exit(1);
    }
        
}

void allocate_memory(list_t * freelist, list_t * alloclist, int pid, int blocksize, int policy) {

    node_t *blocknode = NULL; 

    node_t *currentNode = freelist->head;
    node_t *prev = NULL; 
    while(currentNode){
      if (currentNode->blk->end - currentNode->blk->start >= blocksize){
        blocknode = currentNode; 
        break;
      } 
      prev = currentNode;
      currentNode = currentNode->next;
    }

    
    if (!blocknode){
      printf("Memory allocation Error");
      return;
    }
    else { 
      if (!prev){
        freelist->head = freelist->head->next;   
      } else{
        prev->next = blocknode->next;
      }
    }
  
    int blk_end = blocknode->blk->end; 

    
    blocknode->blk->pid = pid;
  
 
    blocknode->blk->end = blocknode->blk->start + blocksize - 1;
  
   
    list_add_ascending_by_address(alloclist, blocknode->blk);

  
    if (blocknode->blk->end != blk_end){
    
      block_t *fragment = (block_t*)malloc(sizeof(block_t));
      

      fragment->pid = 0;
      
   
      fragment->start = blocknode->blk->end + 1;
      
     
      fragment->end = blk_end;

 
      if (policy == 1){
        list_add_to_back(freelist, fragment);
      }
      else if (policy == 2){
        list_add_ascending_by_blocksize(freelist, fragment);
      }
      else if (policy == 3){
        list_add_descending_by_blocksize(freelist, fragment);
      }
    }
}

void deallocate_memory(list_t * alloclist, list_t * freelist, int pid, int policy) { 

    node_t *currentNode = alloclist->head;
    node_t *prev = NULL;
    block_t *deallocate_blk = NULL;
 
    while (currentNode && currentNode->blk->pid != pid) {
      prev = currentNode;
      currentNode = currentNode->next;
    }
  
    if (!currentNode){
      printf("Error: Can't locate Memory Used by PID");
      return;
    }
    
    
    if (!prev){
      alloclist->head = alloclist->head->next;
      deallocate_blk = currentNode->blk;
    }
    else{
      prev->next = currentNode->next;
      deallocate_blk = currentNode->blk;
    }
    deallocate_blk->pid = 0;

     
    if (policy == 1){
      list_add_to_back(freelist, deallocate_blk);
    }
    else if (policy == 2){
      list_add_ascending_by_blocksize(freelist, deallocate_blk);
    }
    else if (policy == 3){
      list_add_descending_by_blocksize(freelist, deallocate_blk);
    }
}

list_t* coalese_memory(list_t * list){
  list_t *temp_list = list_alloc();
  block_t *blk;
  
  while((blk = list_remove_from_front(list)) != NULL) { 
        list_add_ascending_by_address(temp_list, blk);
  }
  

  
  list_coalese_nodes(temp_list);
        
  return temp_list;
}

void print_list(list_t * list, char * message){
    node_t *current = list->head;
    block_t *blk;
    int i = 0;
  
    printf("%s:\n", message);
  
    while(current != NULL){
        blk = current->blk;
        printf("Block %d:\t START: %d\t END: %d", i, blk->start, blk->end);
      
        if(blk->pid != 0)
            printf("\t PID: %d\n", blk->pid);
        else  
            printf("\n");
      
        current = current->next;
        i += 1;
    }
}

/* DO NOT MODIFY */
int main(int argc, char *argv[]) 
{
   int PARTITION_SIZE, inputdata[200][2], N = 0, Memory_Mgt_Policy;
  
   list_t *FREE_LIST = list_alloc();   // list that holds all free blocks (PID is always zero)
   list_t *ALLOC_LIST = list_alloc();  // list that holds all allocated blocks
   int i;
  
   if(argc != 3) {
       printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
       exit(1);
   }
  
   get_input(argv, inputdata, &N, &PARTITION_SIZE, &Memory_Mgt_Policy);
  
   // Allocated the initial partition of size PARTITION_SIZE
   
   block_t * partition = malloc(sizeof(block_t));   // create the partition meta data
   partition->start = 0;
   partition->end = PARTITION_SIZE + partition->start - 1;
                                   
   list_add_to_front(FREE_LIST, partition);          // add partition to free list
                                   
   for(i = 0; i < N; i++) // loop through all the input data and simulate a memory management policy
   {
       printf("************************\n");
       if(inputdata[i][0] != -99999 && inputdata[i][0] > 0) {
             printf("ALLOCATE: %d FROM PID: %d\n", inputdata[i][1], inputdata[i][0]);
             allocate_memory(FREE_LIST, ALLOC_LIST, inputdata[i][0], inputdata[i][1], Memory_Mgt_Policy);
       }
       else if (inputdata[i][0] != -99999 && inputdata[i][0] < 0) {
             printf("DEALLOCATE MEM: PID %d\n", abs(inputdata[i][0]));
             deallocate_memory(ALLOC_LIST, FREE_LIST, abs(inputdata[i][0]), Memory_Mgt_Policy);
       }
       else {
             printf("COALESCE/COMPACT\n");
             FREE_LIST = coalese_memory(FREE_LIST);
       }   
     
       printf("************************\n");
       print_list(FREE_LIST, "Free Memory");
       print_list(ALLOC_LIST,"\nAllocated Memory");
       printf("\n\n");
   }
  
   list_free(FREE_LIST);
   list_free(ALLOC_LIST);
  
   return 0;
}