#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int weight;
    int pos[2];
} mhitem;

typedef struct{
    int heap_length;
    mhitem* heap_array;
} minheap;


minheap new_minheap(){
    minheap new_heap;
    new_heap.heap_length = 0;
    new_heap.heap_array = malloc(0);
    return new_heap;
}

void free_minheap(minheap* mh){
    free( mh->heap_array );
}

void minheap_swap(minheap* mh, int index1, int index2){
    mhitem temp = mh->heap_array[index1];
    mh->heap_array[index1] = mh->heap_array[index2];
    mh->heap_array[index2] = temp;
}

void minheap_heapify(minheap* mh,int index){
    if (mh->heap_length < 2) {
        return; 
    }
    for (;;) {
        int left_child = 2*index+1;
        int right_child = 2*index+2;

        int smallest = index;
        bool has_space_left = left_child < mh->heap_length;
        bool has_space_right = right_child < mh->heap_length;
        if (has_space_left && mh->heap_array[left_child].weight < mh->heap_array[smallest].weight) {
            smallest = left_child; 
        }
        if (has_space_right && mh->heap_array[right_child].weight < mh->heap_array[smallest].weight) {
            smallest = right_child; 
        }

        if (smallest != index) {
            minheap_swap(mh,smallest,index);
            index = smallest;
        }else {
            break; 
        }
    }
}

void minheap_insert(minheap* mh, mhitem item){
    mh->heap_length++;
    mh->heap_array = realloc(mh->heap_array, sizeof(mhitem)*mh->heap_length);
    mh->heap_array[mh->heap_length-1] = item;
    for (int i = mh->heap_length-1; i > 0;) {
        int i_parent = (i-2)/2;
        mhitem current = mh->heap_array[i];
        mhitem parent = mh->heap_array[i_parent];
        if (current.weight < parent.weight) {
            minheap_swap(mh, i,i_parent);
        }else{
            break;
        }
        i = i_parent;
    }
}

void minheap_delete(minheap* mh){
    if (mh->heap_length == 0) {
        return; 
    }
    mhitem last = mh->heap_array[mh->heap_length-1];
    mh->heap_array[0] = last;
    mh->heap_length--;
    minheap_heapify(mh, 0);
}

mhitem minheap_extract(minheap* mh){
    mhitem item = mh->heap_array[0];
    minheap_delete(mh);
    return item;
}

int main(){
    FILE* input;
    fopen_s(&input, "test.txt", "rb");

    fseek(input, 0, SEEK_END);
    size_t file_size = ftell(input);
    rewind(input);

    char* map = malloc(file_size);
    fread(map, sizeof(char) , file_size, input);
    map[file_size-1] = '\0';
    fclose(input);

    printf("%s\n",map);

    free(map);
}
