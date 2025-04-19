#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


#define TESTING
#ifdef TESTING
    #define SQR_SIZE 7
    #define INPUT "test.txt"
    #define SIM 12
#else
    #define SQR_SIZE 71
    #define INPUT "q18.txt"
    #define SIM 1024
#endif

int square_size = SQR_SIZE;
char gridAtlas[] = ".#O";

void printGrid(int* grid){
    for (int i = 0; i < square_size*square_size; i++) {
        printf("%c",gridAtlas[grid[i]]);
        if ((i+1)%square_size == 0) {
            printf("\n");
        }
    }
}

int main(){
    FILE* input;
    fopen_s(&input,INPUT,"rb");

    fseek(input, 0, SEEK_END);
    size_t read_count = ftell(input);
    rewind(input);

    char* raw_coords = malloc(read_count+1);
    raw_coords[read_count] = '\0';
    fread(raw_coords, sizeof(char), read_count, input);
    fclose(input);

    int* memory = calloc(square_size*square_size, sizeof(int));


    char* line_end = strchr(raw_coords, '\n');
    char* line_start = raw_coords;
    for (int i = 0; i < SIM; i++) {
        char* sep = strchr(line_start, ',');
        *sep = '\0';
        int x = atoi(line_start);
        *line_end = '\0';
        int y = atoi(sep+1);

        memory[y*(square_size)+x] = 1;

        if (line_end != NULL) {
            line_start = line_end+1;
            line_end = strchr(line_end+1, '\n');
        }
    }
    printGrid(memory);
    free(raw_coords);
}
