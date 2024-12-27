#include <corecrt.h>
#include <math.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool inArrayC(int check[2], int ( *array )[2], int n){
    bool found = false;
    for(size_t i = 0; i<n ;i++){
        int* current = array[i];
        bool xcheck = current[0] == check[0];
        bool ycheck = current[1] == check[1];
        if(xcheck && ycheck){
            found = true;
            break;
        }
    }
    return found;
}
int main() {
    FILE *inputs;
    errno_t err = fopen_s(&inputs,"test.txt", "rb");

    int pos[2] = {0,0};
    int y_size = 0;
    int x_size = 0;

    int mapn = 0;
    char* map = malloc(0);
    for (;;) {
        char buffer[1024];
        char* r = fgets(buffer, 1024, inputs);
        if (r == NULL) {
            break; 
        }
        int n = strlen(buffer);
        int oldmapn = mapn;
        mapn += 2*n-1;
        map = realloc(map,sizeof(char)*mapn);
        for (int i = 0; i < n; i++) {
            char cchar = buffer[i];
            int map_index = ( i*2 )+oldmapn;
            switch (cchar) {
                case '#': 
                    map[map_index] = '#';
                    map[map_index+1] = '#';
                    break;
                case 'O':
                    map[map_index] = '[';
                    map[map_index+1] = ']';
                    break;
                case '.':
                    map[map_index] = '.';
                    map[map_index+1] = '.';
                    break;
                case '@':
                    pos[0] = i*2;
                    pos[1] = y_size;
                    map[map_index] = '@';
                    map[map_index+1] = '.';
                    break;
                case '\n':
                    map[map_index] = '\n';
            }
        }
        if (buffer[0] == '\n') {
            break; 
        }
        y_size+=1;
    }
    x_size = strchr(map, '\n')-map;
    map = realloc(map,sizeof(char)*(mapn+1));
    map[mapn-1] = '\0';

    size_t current_position = ftell(inputs);
    fseek(inputs, 0, SEEK_END);
    size_t end_position = ftell(inputs);
    size_t read_count = end_position-current_position;
    fseek(inputs, current_position, SEEK_SET);
    char* instructions = malloc(sizeof(char)*(read_count+1));
    fread(instructions,sizeof(char),read_count,inputs);
    read_count--;
    instructions[read_count] = '\0';
    // direction chr codes
    // <  60
    // v 118
    // > 62
    // ^ 94

    // plan
    // for every direction command 
    // get direction vector
    // read in data in front of robot in direction vector direction (everything in front of robot goes into a list)
    // stop at termination chracter # or . (wall or space)
    // if wall => continue to next direction command
    // if space => repalce space with last thing read in before it. Replace robot with space. And if box in front of robot's old position repalce it with the robot
    int directions[4][2] = {{-1,0},{0,1},{1,0},{0,-1}};
    for (int i = 0; i < read_count; i++) {
        char command = instructions[i];
        if (command == '\n') {
            continue;
        }
        int direction_index = 0;
        switch (command) {
            case '<': 
                direction_index = 0;
                break;
            case 'v':
                direction_index = 1;
                break;
            case '>':
                direction_index = 2;
                break;
            case '^':
                direction_index = 3;
                break;
        } 
        int* direction_vec = directions[direction_index];
        int termination_index = 0;
        int queue_size = 1;
        int queue_pointer = 0;
        int (*queue)[2] = malloc(sizeof(int[2]));
        queue[0][0] = pos[0];
        queue[0][1] = pos[1];
        int foundWall = false;
        for (;;) {
            int current_queue_size = queue_size;
            for (int i = queue_pointer; i < current_queue_size; i++) {
                int* current_pos = queue[i];
                int current_index = current_pos[1]*(x_size+1)+current_pos[0];
                int next_pos[2] = {current_pos[0]+direction_vec[0],current_pos[1]+direction_vec[1]};
                int next_index = next_pos[1]*(x_size+1)+next_pos[0];
                if (map[next_index] == '.') {
                    continue; 
                }
                if (map[next_index] == '#') {
                    foundWall = true; 
                    break;
                }
                bool inarray = inArrayC(next_pos, &queue[queue_pointer], queue_size-queue_pointer);
                if (!inarray) {
                    queue_size++;
                    queue = realloc(queue,sizeof(int[2])*queue_size);
                    queue[queue_size-1][0] = next_pos[0];
                    queue[queue_size-1][1] = next_pos[1];
                    // top down special handling for multi box push 
                    char current_box = map[current_index];
                    char next_box = map[next_index];
                    if (( direction_index == 1 || direction_index == 3 ) && (next_box != current_box)) {
                        // cur [ next ] deviation = x-1
                        // cur ] enxt [ deviation = x+1
                        queue_size++;
                        queue = realloc(queue,sizeof(int[2])*queue_size);
                        if (next_box == '[') {
                            queue[queue_size-1][0] = next_pos[0]+1;
                        }else if (next_box == ']') {
                            queue[queue_size-1][0] = next_pos[0]-1;
                        }
                        queue[queue_size-1][1] = next_pos[1];
                    }
                }
            }
            if (foundWall) {
                break; 
            }
            queue_pointer = current_queue_size;
            if (current_queue_size == queue_size) {
                break; 
            }
        }
        if (!foundWall) { //perform the moving forward of all the boxes & robot by recursing through the queue backwards
             for (int i = queue_size-1; i >= 0; i--) {
                int* cur_q_item = queue[i];
                int current_index = cur_q_item[1]*(x_size+1) + cur_q_item[0];
                int next_pos[2] = {cur_q_item[0]+direction_vec[0],cur_q_item[1]+direction_vec[1]};
                int next_index = next_pos[1]*(x_size+1)+next_pos[0];
                map[next_index] = map[current_index];
                map[current_index] = '.';
                if (map[next_index] == '@') {
                    pos[0] = next_pos[0];
                    pos[1] = next_pos[1];
                }
             }
        }
        free(queue);
    }
    // printf("%s",map);
    int cord_sum = 0;
    for (int i = 0; i < mapn; i++) {
        if(map[i] == '['){
            cord_sum += i/(x_size+1)*(99-x_size)+i;
        }
    }
    printf("%d\n",cord_sum);
    free(map);
    fclose(inputs);
}
