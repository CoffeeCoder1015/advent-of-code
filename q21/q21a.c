#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Nunber pad:
// 'A' mapped to index 10, so that the rest of the numbers are a perfect hash
//
//Y=
// 0|7 8 9
// 1|4 5 6
// 2|1 2 3
// 3|  0 A
//  ------
//X=[0 1 2]
//
//
//
//
//                            0     1     2     3     4     5     6     7     8     9     A
int numpad_coords[11][2] = {{1,3},{0,2},{1,2},{2,2},{0,1},{1,1},{2,1},{0,0},{1,0},{2,0},{2,3}};

//Direction pad:
// 'A' mapped to the end so the directons coords are a perfect hash to the previous questions' dir vector list
//
//Y=
// 0|   ^ A
// 1| < v >
//  -------
//X= [0 1 2]
//                           >     v     <     ^     A
int dirpad_coords[5][2] = {{2,1},{1,1},{0,1},{1,0},{2,0}};
char dir_glyph[] = ">v<^A";
int directions[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};

typedef struct{
    int size;
    int* ptr;
} array;

int numpad_map(int x){
    if (x == 'A') {
        return 10; 
    }
    return x - '0';
}

array to_robot_instructions(int mode, int* instructions, int n){
    int base_capacity = 10;
    int base_size = 0;
    int* base_array = malloc(base_capacity*sizeof(int));

    int previous_mapping = 4;  
    if (mode == 0) {
        previous_mapping = 10; 
    }

    for (int i = 0; i < n; i++) {
        int current_mapping = instructions[i];
        int* current = dirpad_coords[current_mapping];
        int* previous = dirpad_coords[previous_mapping];
        if (mode == 0) {
            current_mapping = numpad_map(current_mapping);
            current = numpad_coords[current_mapping];
            previous = numpad_coords[previous_mapping];
        }

        int vec_diff[2] = { current[0]-previous[0], current[1]-previous[1] };
        // printf("M:%d %d,%d -> %d,%d\n",current_mapping,previous[0],previous[1],current[0],current[1]);
        // printf("%c %d,%d\n",instructions[i],vec_diff[0],vec_diff[1]);

        int x_mag = abs(vec_diff[0]);
        int y_mag = abs(vec_diff[1]);

        int memory_defecit = base_size + x_mag + y_mag + 2  - base_capacity;
        if (memory_defecit > 0) {
            base_capacity += abs(memory_defecit);
            base_array  = realloc(base_array, sizeof(int)*base_capacity);
        }

        int applyx[2] = {current[0],previous[1]};
        int applyy[2] = {previous[0],current[1]};
        
        bool dirpadhit_nsx = applyx[0] == 0 && applyx[1] == 0;
        bool dirpadhit_nsy = applyy[0] == 0 && applyy[1] == 0;
        bool reverse = dirpadhit_nsx || dirpadhit_nsy; 
        if(mode==0){ 
            bool numpadhit_nsx = applyx[0] == 0 && applyx[1] == 3;
            bool numpadhit_nsy = applyy[0] == 0 && applyy[1] == 3;
            reverse = numpadhit_nsx || numpadhit_nsy; 
        }


        int xneg = vec_diff[0]>>31 | !!vec_diff[0];
        int yneg = vec_diff[1]>>31 | !!vec_diff[1];
        int x_index = 1-xneg;
        int y_index = 2-yneg;

        bool xfirst = xneg==-1 && !reverse || xneg >= 0 && reverse;

        if (xfirst) {
            for (int j = 0; j < x_mag; j++) {
                base_size++; 
                base_array[base_size-1] = 1-xneg;
            }
        }

        for (int j = 0; j < y_mag; j++) {
            base_size++; 
            base_array[base_size-1] = 2-yneg;
        }

        if (!xfirst) {
            for (int j = 0; j < x_mag; j++) {
                base_size++; 
                base_array[base_size-1] = 1-xneg;
            }
        }

        base_size++;
        base_array[base_size-1] = 4;
        previous_mapping = current_mapping;
    }
    return (array){base_size,base_array} ;
}

void debug_print(array a){
    // for (int i = 0; i < a.size; i++) {
    //     printf("%d,",a.ptr[i]);
    // }
    // printf("\n");
    for (int i = 0; i < a.size; i++) {
        printf("%c",dir_glyph[a.ptr[i]]);
    }
    printf("\n");
}


int main(){
    FILE* input;
    fopen_s(&input, "q21.txt", "rb");

    for (;;) {
        char buffer[1024];
        char* end = fgets(buffer, 1024 , input);
        if (end == NULL) {
            break; 
        }
        int n = strlen(buffer);
        buffer[--n] = '\0';

        printf("%s\n",buffer);

        int ibuffer[1024];
        for (int i = 0; i < n; i++) {
            ibuffer[i]  = buffer[i];
        }

        array l1 = to_robot_instructions(0, ibuffer, n);
        array l2 = to_robot_instructions(1, l1.ptr, l1.size);
        array l3 = to_robot_instructions(1, l2.ptr, l2.size);
        debug_print(l1);
        debug_print(l2);
        debug_print(l3);
        printf("%d %d %d\n",l1.size,l2.size,l3.size);

        free(l1.ptr);
        free(l2.ptr);
        free(l3.ptr);
        char* termination = strchr(buffer,'A');
        *termination = '\0';
        int interger_component = atoi(buffer);
        printf("%d\n",interger_component);
    }
}
