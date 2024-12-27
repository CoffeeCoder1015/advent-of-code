#include <corecrt.h>
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

    printf("%s",map);
    printf("%d %d %d,%d",x_size,y_size,pos[0],pos[1]);
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
    for (;;) {
        char buffer[1024];
        char* r = fgets(buffer, 1024, inputs);
        if (r == NULL) {
            break; 
        }
        for (int i = 0; buffer[i] != '\n'; i++) {
            char command = buffer[i];
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
            for (int mult = 0;;mult++) {
                int next_pos[2]  = {pos[0]+direction_vec[0],pos[1]+direction_vec[1]};
                int next_index = next_pos[1]*(x_size+1)+next_pos[0];
            }
        }
    }


    free(map);
    fclose(inputs);
}
