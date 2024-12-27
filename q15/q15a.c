#include <corecrt.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
    FILE *inputs;
    errno_t err = fopen_s(&inputs,"test.txt", "rb");

    size_t last_len;
    int pos[2] = {0,0};
    int y_size = 0;
    int x_size = 0;

    for (;;) {
        char buffer[1024];
        char* r = fgets(buffer, 1024, inputs);
        if (r == NULL) {
            break; 
        }
        // printf("%s %llu %d",buffer,strlen(buffer),buffer[0]);
        char* robot_pos = strchr(buffer, '@');
        if (robot_pos != NULL) {
            pos[0] = robot_pos-buffer;
            pos[1] = y_size;
        }

        if (buffer[0] == '\n') {
            break; 
        }
        y_size+=1;
        last_len = strlen(buffer);
    }
    size_t ptr_pos = ftell(inputs)-1;
    rewind(inputs);
    char* map = malloc(sizeof(char)*( ptr_pos+1 ));
    fread(map, sizeof(char), ptr_pos+1, inputs);
    map[ptr_pos] = '\0';

    x_size = strchr(map, '\n')-map;

    size_t current_position = ftell(inputs);
    fseek(inputs, 0, SEEK_END);
    size_t end_position = ftell(inputs);
    size_t read_count = end_position-current_position;
    fseek(inputs, current_position, SEEK_SET);
    char* instructions = malloc(sizeof(char)*(read_count+1));
    fread(instructions,sizeof(char),read_count,inputs);
    instructions[read_count-1] = '\0';
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
        int current_index = pos[1]*(x_size+1)+pos[0];
        bool isWall = false;
        bool last_is_box = false;
        for (int mult = 1;;mult++) {
            int next_pos[2]  = {pos[0]+direction_vec[0]*mult,pos[1]+direction_vec[1]*mult};
            int next_index = next_pos[1]*(x_size+1)+next_pos[0];
            bool wall_found = map[next_index] == '#';
            if (wall_found || map[next_index] == '.') {
                termination_index = next_index;
                int previous_pos[2] = {next_pos[0]-direction_vec[0],next_pos[1]-direction_vec[1]};
                int previous_index = previous_pos[1]*(x_size+1)+previous_pos[0];
                last_is_box = map[previous_index] == 'O';
                isWall = wall_found;
                break;
            }
        }
        if (!isWall) {
            map[current_index] = '.';
            pos[0] += direction_vec[0];
            pos[1] += direction_vec[1];
            current_index = pos[1]*(x_size+1)+pos[0];
            map[current_index] = '@';
            if (last_is_box) {
                map[termination_index] = 'O';
            } 
        }
    }
    // printf("%s",map);
    int cord_sum = 0;
    for (int i = 0; i < ptr_pos; i++) {
        if(map[i] == 'O'){
            cord_sum += i/(x_size+1)*(99-x_size)+i;
        }
    }
    printf("%d\n",cord_sum);
    free(map);
    fclose(inputs);
}
