#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Nunber pad:
// 'A' mapped to index 10, so that the rest of the numbers are a perfect hash
//
//Y=
// 3|7 8 9
// 2|4 5 6
// 1|1 2 3
// 0|  0 A
//  ------
//X=[2 1 0]
//
//
//
//
//                            0     1     2     3     4     5     6     7     8     9     A
int numpad_coords[11][2] = {{1,0},{2,1},{1,1},{0,1},{2,2},{1,2},{0,2},{2,3},{1,3},{0,3},{0,0}};

//Direction pad:
// 'A' mapped to the end so the directons coords are a perfect hash to the previous questions' dir vector list
//
//Y=
// 0|   ^ A
// 1| < v >
//  -------
//X= [2 1 0]
//                           <     ^     >     v     A
int dirpad_coords[5][2] = {{2,1},{1,1},{0,1},{1,1},{0,0}};
char dir_glyph[] = "<^>vA";
int directions[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};

int main(){
    FILE* input;
    fopen_s(&input, "test.txt", "rb");

    for (;;) {
        char buffer[1024];
        char* end = fgets(buffer, 1024 , input);
        if (end == NULL) {
            break; 
        }
        int n = strlen(buffer);
        buffer[--n] = '\0';

        printf("%s\n",buffer);
        int base_capacity = 10;
        int base_size = 0;
        int* base_array = malloc(base_capacity*sizeof(int));

        int previous_mapping = 10;  // mapping for 'A'
        for (int i = 0; i < n; i++) {
            int current_mapping = buffer[i] - '0';
            if (buffer[i] == 'A') {
                current_mapping = 10; 
            }

            int* current = numpad_coords[current_mapping];
            int* previous = numpad_coords[previous_mapping];
            int vec_diff[2] = { current[0]-previous[0], current[1]-previous[1] };
            // printf("M:%d %d,%d -> %d,%d\n",current_mapping,previous[0],previous[1],current[0],current[1]);
            // printf("%c %d,%d\n",buffer[i],vec_diff[0],vec_diff[1]);

            int x_mag = abs(vec_diff[0]);
            int y_mag = abs(vec_diff[1]);

            int memory_defecit = base_size + x_mag + y_mag + 2  - base_capacity;
            if (memory_defecit > 0) {
                base_capacity += abs(memory_defecit);
                base_array  = realloc(base_array, sizeof(int)*base_capacity);
            }

            int xneg = vec_diff[0]>>31 | !!vec_diff[0];
            for (int j = 0; j < x_mag; j++) {
                base_size++; 
                base_array[base_size-1] = 1-xneg;
            }

            int yneg = vec_diff[1]>>31 | !!vec_diff[1];
            for (int j = 0; j < y_mag; j++) {
                base_size++; 
                base_array[base_size-1] = 2-yneg;
            }

            base_size++;
            base_array[base_size-1] = 4;
            previous_mapping = current_mapping;
        }

        for (int i = 0; i < base_size; i++) {
            printf("%c",dir_glyph[base_array[i]]) ;
        }
        printf("\n");

        free(base_array);
        char* termination = strchr(buffer,'A');
        *termination = '\0';
        int interger_component = atoi(buffer);
        printf("%d\n",interger_component);
    }
}
