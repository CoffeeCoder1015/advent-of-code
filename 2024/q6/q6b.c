#include <math.h>
#include <stdlib.h>
#include <corecrt.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct{
    int start;
    int end;
    int direction;
} segment;


int directions[4][2] = {{0,-1},{1,0},{0,1},{-1,0}};
int main(){
    FILE* inputs; 
    errno_t err = fopen_s(&inputs,"q6.txt", "r");

    fseek(inputs, 0, SEEK_END);
    size_t character_count = ftell(inputs);
    rewind(inputs);

    size_t alloc_size = sizeof(char)*(character_count+1);
    char* buffer = (char*)malloc(alloc_size);
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


    char* tmp_buffer = malloc(alloc_size);
    strncpy_s(tmp_buffer,alloc_size, buffer,character_count);

    int pos[2] = {x,y};
    int current_direction = 0;
    int indexes_count = 0;
    int* try_obstacles= malloc(0);
    bool looped = false;
    int traversals = 1;
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
            dir_vec = directions[current_direction];
            next_pos[0] = dir_vec[0]+pos[0];
            next_pos[1] = dir_vec[1]+pos[1];
            next_index = next_pos[1]*(x_size+1)+next_pos[0];
            next  = buffer[next_index];
        }

        if(next == '.'){
            indexes_count++;
            try_obstacles = realloc(try_obstacles, sizeof(int)*indexes_count);
            try_obstacles[indexes_count-1] = next_index;
            buffer[next_index] = '0';
        }
        pos[0] = next_pos[0];
        pos[1] = next_pos[1];
    }
    free(buffer);
    buffer = tmp_buffer;

    int total_looped = 0;

    for (size_t I =0;I<indexes_count;I++) {
        int block_index = try_obstacles[I];
        buffer[block_index] = '#';

        int pos[2] = {x,y};

        int current_direction = 0;

        int paths_count = 0;
        segment* refernce_path = malloc(0);
        int mult = 1;
        bool looped = false;
        for (size_t i =0;i<x_size*y_size;i++) {
            int* dir_vec = directions[current_direction];
            int next_pos[2] = {mult*dir_vec[0]+pos[0],mult*dir_vec[1]+pos[1]};
            int nextx = next_pos[0]-dir_vec[0];
            int nexty = next_pos[1]-dir_vec[1];

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
                int next_index = nexty*(x_size+1)+nextx;
                int current_index = pos[1]*(x_size+1)+pos[0];
                segment current_seg = {current_index,next_index,current_direction};

                for (size_t k = 0; k < paths_count; k++) {
                    segment loop_seg = refernce_path[k];
                    if(loop_seg.start == current_seg.start && loop_seg.end == current_seg.end && loop_seg.direction == current_seg.direction){
                        looped = true;
                        break;
                    }
                }
                if(looped){
                    total_looped++;
                    break;
                }
                paths_count++;
                refernce_path = realloc(refernce_path, paths_count*sizeof(segment));
                refernce_path[paths_count-1] = current_seg;
                pos[0] = nextx;
                pos[1] = nexty;
                current_direction++;
                current_direction %= 4;
                mult = 1;
                continue;
            }
            mult++;
        }
        buffer[block_index] = '.';
        free(refernce_path);
    }

    printf("%d\n",total_looped);
    free(buffer);
    free(try_obstacles);
}
