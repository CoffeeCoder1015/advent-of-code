#include <corecrt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// referncing https://web.stanford.edu/class/archive/cs/cs161/cs161.1168/lecture4.pdf
// implementing heap insertion, heap deletion 
// heap inc/dec key is also not needed
// heap extractkey is just a compound action of get array[0] and dropping array[0]
// and reheapiydying
//

typedef struct{
    int potential;
    int pos[2];
} mheap_item;

typedef struct{
    mheap_item* heap;
    int length;
} minheap;

void check_heap(minheap* mh){
    int length = mh->length;
    bool good = true;
    for (int i = 0;i < length; i++) {
        mheap_item current_node = mh->heap[i];
        int current_potential = current_node.potential;
        int child1_index = 2*i;
        int child2_index = child1_index+1;
        if (child1_index < length) {
            int child_potential = mh->heap[child1_index].potential;
            if (current_potential > child_potential) {
                good = false ;
                printf("Failed at node [%d]: Parent=%d Child1=%d\n",i,current_potential,child_potential);
            } 
        }
        if (child2_index < length) {
            int child_potential = mh->heap[child2_index].potential;
            if (current_potential > child_potential) {
                good = false ;
                printf("Failed at node [%d]: Parent=%d Child2=%d\n",i,current_potential,child_potential);
            } 
        }
    }
    printf("Heap is %s\n",good? "good" : "bad") ;
}

minheap new_minheap(){
    minheap new_heap;
    new_heap.length = 0;
    new_heap.heap = malloc(0);
    return new_heap;
}

void free_minheap(minheap* mh){
    free( mh->heap );
}

void minheap_swap_index(minheap* mh, int index1, int index2){
    mheap_item temp = mh->heap[index1];
    mh->heap[index1] = mh->heap[index2];
    mh->heap[index2] = temp;
}

// for a heap node in an array at index i
// a left child node would be at 2i
// a right child node would be at 2i+1
// a parent node would be at floor(i/2)
void minheap_insert(minheap* mh, mheap_item item){
    mh->length++;
    int length = mh->length;
    mh->heap = realloc(mh->heap, sizeof(mheap_item)*length);
    mh->heap[length-1] = item;
    // reorder heap
    for (int i = length-1; i > 0; i = (i-1)/2) {
        mheap_item current = mh->heap[i];
        mheap_item parent = mh->heap[i/2];
        if (parent.potential > current.potential) {
            minheap_swap_index(mh,i , i/2);
        }else {
            break;
        }
    }
}

void minheap_heapify(minheap* mh, int index){
    int length = mh->length;
    if (length <= 1) {
        return; 
    }
    int start_index = index;
    for (;;) {
        int left_index = 2*start_index+1;
        int right_index = left_index+2;

        int smallest_index = start_index;
        if (left_index < length && mh->heap[left_index].potential < mh->heap[smallest_index].potential) {
            smallest_index = left_index; 
        }

        if (right_index < length &&  mh->heap[right_index].potential < mh->heap[smallest_index].potential ) {
            smallest_index = right_index; 
        }
        if (smallest_index != start_index) {
            minheap_swap_index(mh,start_index,smallest_index);
            start_index = smallest_index;
        }else {
            break;
        }
    }
}

void minheap_delte(minheap* mh){
    int length = mh->length;
    mheap_item last = mh->heap[length-1];
    mh->heap[0] = last;
    mh->length--;
    length--;
    minheap_heapify(mh, 0);
}

mheap_item minheap_extract(minheap* mh){
    mheap_item item = mh->heap[0];
    minheap_delte(mh);
    return item;
}

int main(){
    FILE* inputs;
    errno_t err = fopen_s(&inputs , "test.txt", "rb");
    fseek(inputs, 0, SEEK_END);
    size_t read_count = ftell(inputs);
    rewind(inputs);
    char* buffer = malloc(sizeof(char)*(read_count+1));
    fread(buffer,sizeof(char),read_count,inputs);
    buffer[read_count] = '\0';
    char* nline = strchr(buffer, '\n');

    int x_size = nline-buffer;
    int y_size = 0;

    while (nline!=NULL) {
        y_size++;
        nline=strchr(nline+1,'\n');
    }
    int S_index = strchr(buffer,'S')-buffer;
    int E_index = strchr(buffer,'E')-buffer;
    int spos[2] = {0,S_index/x_size};
    spos[0] = S_index - spos[1]*(x_size+1);
    int epos[2] = {0,E_index/y_size};
    epos[0] = E_index - epos[1]*(x_size+1);

    int directions[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
    int current_direction = 0;

    for (;;) {
    
    }

    free(buffer);
}
