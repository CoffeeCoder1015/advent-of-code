#include <corecrt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool inArray(int check, int* array, int n){
    bool found = false;
    for(size_t i = 0; i<n ;i++){
        int current = array[i];
        // printf("%d == %d\n",check,curent);
        if(check == current){
            found = true;
            break;
        }
    }
    return found;
}

int main(){
    FILE* inputs; 
    errno_t err = fopen_s(&inputs,"q10.txt", "rb");

    fseek(inputs, 0, SEEK_END);
    size_t character_count = ftell(inputs);
    rewind(inputs);

    size_t alloc_size = sizeof(char)*(character_count);
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

    int trail_start_count = 0;
    int (*trail_start)[2] = malloc(0);
    for (int i = 0; i < character_count; i++) {
        if (buffer[i] == '0') {
            trail_start_count++;   
            trail_start = realloc(trail_start, sizeof(int[2])*trail_start_count);
            int y = i/(x_size+1);
            int x = i - y*(x_size+1);
            trail_start[trail_start_count-1][0] = x;
            trail_start[trail_start_count-1][1] = y;
        } 
    }


    // int* visited = malloc(0);
    int stack_count = 0;
    int ( *stack )[2] = malloc(0);
    int END_COUNT = 0;
    for (int i = 0; i < trail_start_count; i++) {

        // //visited is reset for every train start, no need to malloc again 
        // //as resetting the size will allow realloc() to do it automatically
        // int visit_count = 0;

        int* pos = trail_start[i];
        int sec = END_COUNT;
        int target = 1;
        // start the stack
        stack_count++;
        stack  = realloc(stack,sizeof(int[2])*stack_count);
        stack[stack_count-1][0] = pos[0];
        stack[stack_count-1][1] = pos[1];
        for (;stack_count!=0; ) {
            //get the stack value
            int* p_stack = stack[stack_count-1];
            int current_index = p_stack[1]*(x_size+1)+p_stack[0];
            int current = buffer[current_index]-48;
            target = current+1;
            //consume the stack value
            stack_count--;

            int possible_pos[4][2] = {
                {p_stack[0],p_stack[1]-1},
                {p_stack[0],p_stack[1]+1},
                {p_stack[0]-1,p_stack[1]},
                {p_stack[0]+1,p_stack[1]}
            };
            // printf("--- %d,%d c:%d---\n",p_stack[0],p_stack[1],current);
            for (int j =0; j < 4; j++) {
                int* selected_pos = possible_pos[j];
                bool is_in_X = 0<=selected_pos[0] && selected_pos[0]<x_size;
                bool is_in_Y = 0<=selected_pos[1] && selected_pos[1]<y_size;
                if (is_in_X && is_in_Y) {
                    int selection_index = selected_pos[1]*(x_size+1)+selected_pos[0];
                    int selected = buffer[selection_index]-48;
                    if(selected == target){
                        if (selected == 9) {
                            // bool already_visited = inArray(selection_index, visited, visit_count);
                            // if (!already_visited) {
                            //     visit_count++; 
                            //     visited = realloc(visited, sizeof(int)*visit_count);
                            //     visited[visit_count-1] = selection_index;
                            // }
                            END_COUNT++; 
                            continue;
                        }
                        // printf("%d %d: %d,%d\n",selected,target,selected_pos[0],selected_pos[1]);
                        //add to stack
                        stack_count++;
                        stack  = realloc(stack,sizeof(int[2])*stack_count);
                        stack[stack_count-1][0] = selected_pos[0];
                        stack[stack_count-1][1] = selected_pos[1];
                    }
                }
            }
        }
        stack_count = 0;
    }
    printf("%d\n",END_COUNT);
    free(stack);
    // free(visited);
    free(trail_start);
}
