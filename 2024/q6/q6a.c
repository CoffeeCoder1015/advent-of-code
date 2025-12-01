#include <math.h>
#include <stdlib.h>
#include <corecrt.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int main(){
    FILE* inputs; 
    errno_t err = fopen_s(&inputs,"q6.txt", "r");

    fseek(inputs, 0, SEEK_END);
    size_t character_count = ftell(inputs);
    rewind(inputs);

    size_t alloc_size = sizeof(char)*(character_count+1);
    char* buffer = malloc(alloc_size);
    fread(buffer,sizeof(char),character_count,inputs);

    fclose(inputs);
    buffer[character_count] = '\0';

    char *pch=strchr(buffer,'\n');
    size_t y_size = 0;
    size_t x_size = pch-buffer;

    while (pch!=NULL) {
        y_size++;
        pch=strchr(pch+1,'\n');
    }
    char* guardpt = strchr(buffer,'^');
    *guardpt = 'S';
    int guard_index = guardpt-buffer;

    int y = guard_index/(x_size+1);
    int x = guard_index - y*(x_size+1);
    int pos[2] = {x,y};

    // printf("%d (%d,%d) [%d,%d]\n",guard_index,x,y,x_size,y_size);


    int directions[4][2] = {{0,-1},{1,0},{0,1},{-1,0}};
    int current_direction = 0;

    int traversals = 1; //start at 1 for counting the inital position
    for (size_t i =0;i<x_size*y_size;i++) {
        int* dir_vec = directions[current_direction];
        int next_pos[2] = {dir_vec[0]+pos[0],dir_vec[1]+pos[1]};

        //* Not off map *//
        bool x_in = ( 0<=next_pos[0] ) && ( next_pos[0] < x_size);
        bool y_in = (0<=next_pos[1]) && (next_pos[1] < y_size);
        if(!(x_in && y_in)){
            break;
        }

        //* Not into obstacle *//
        int next_index = next_pos[1]*(x_size+1)+next_pos[0];
        char next = buffer[next_index];
        if(next == '#'){
            current_direction++;
            current_direction %= 4;
            switch (current_direction) {
                case 0:
                    buffer[pos[1]*(x_size+1)+pos[0]] = '^';
                    break;
                case 1:
                    buffer[pos[1]*(x_size+1)+pos[0]] = '>';
                    break;
                case 2:
                    buffer[pos[1]*(x_size+1)+pos[0]] = 'V';
                    break;
                case 3:
                    buffer[pos[1]*(x_size+1)+pos[0]] = '<';
                    break;

            }
            dir_vec = directions[current_direction];
            next_pos[0] = dir_vec[0]+pos[0];
            next_pos[1] = dir_vec[1]+pos[1];
            next_index = next_pos[1]*(x_size+1)+next_pos[0];
            next = buffer[next_index];
        }
        if(next == '.'){
            traversals++;
            buffer[next_index] = '0';
        }
        pos[0] = next_pos[0];
        pos[1] = next_pos[1];
    }
    // printf("%s\n",buffer);
    printf("%d",traversals);
    free(buffer);
}
