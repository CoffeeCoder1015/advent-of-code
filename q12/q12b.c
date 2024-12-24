#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_coord_array(int (*cords)[2], int n){
    for (int i =0; i<n; i++) {
        printf("(%d,%d) ",cords[i][0],cords[i][1]) ;
    }
    printf("\n");
}

// I toiled for a morning to find the sides of shapes,
// and I was in deep rejoice when it told me a square had 4 sides,
// but then my joy was asunder,
// when I realised I missed an edge case.
// -- Me, right now.
//
//
//  1. Aligns against an edge 
//
//  Incorrect- Obstacle in front
//  #
//  ^
//
//  Aligns to-
//  #
//  >
//
//  2. Main movement
//  IF hit wall
//  rotate
//
//  -- for this situaton
//  ####    =>    ####
//    >#    =>      V#
//     #    =>       #
//
//  IF edge disapperas 
//  rotate in other direction
//
//  -- for this situaton
//      #     =>         #
//      #     =>         #
//  #####     =>     ##### 
//       >    =>           ^
// 
// IF didnt hit wall, and edge didnt disappera
// move forward
int EdgeRunner(char* map,int x_size,int y_size, int init[2]){
    printf("---------------(%d,%d)-----------------\n",init[0],init[1]);
    int Sides = 0;
    bool coupled = false;

    int directions[4][2] = {{0,-1},{1,0},{0,1},{-1,0}};
    int current_direction = 0;
    int starting_direction = 0;
    int starting_index = init[1]*(x_size+1)+init[0];
    int pos[2] = {init[0],init[1]};

    // prealign first
    for (int i = 0; i < 3; i++) {
        int* current_side_vec = directions[( current_direction-1 )%4&4-1];
        int current_side[2] = {pos[0]+current_side_vec[0],pos[1]+current_side_vec[1]};
        printf("Got here %d %d\n",current_direction,(current_direction-1)%4&4-1);
        bool x_side_in = ( 0<=current_side[0] ) && ( current_side[0] < x_size);
        bool y_side_in = (0<=current_side[1]) && (current_side[1] < y_size);

        if (!(x_side_in && y_side_in)) {
            starting_direction = current_direction;
            break; 
        }

        int side_index = current_side[1]*(x_size+1)+current_side[0];
        char side = map[side_index];
        if (side != map[starting_index]) {
            starting_direction = current_direction;
            break;
        }

        current_direction++;
        current_direction %= 4;
    }

    printf("%d %d\n",starting_direction,current_direction);
    bool not_start = false;
    for (;;) {
        char visualized;
        switch (current_direction) {
            case 0:
                visualized = '^';
                break;
            case 1:
                visualized = '>';
                break;
            case 2:
                visualized = 'V';
                break;
            case 3:
                visualized = '<';
                break;
        }
        int cur_index = pos[1]*(x_size+1)+pos[0];
        char cur = map[cur_index];
        int* dir_vec = directions[current_direction];
        int next_pos[2] = {dir_vec[0]+pos[0],dir_vec[1]+pos[1]};
        bool x_in = ( 0<=next_pos[0] ) && ( next_pos[0] < x_size);
        bool y_in = (0<=next_pos[1]) && (next_pos[1] < y_size);

        printf("At least we got here %c (%c) %d %d %d\n",visualized, map[cur_index],pos[0],pos[1],Sides);

        if (current_direction == starting_direction && cur_index == starting_index) {
            if (not_start) {
                break; 
            }
            not_start = true;
        }

        if (!(x_in && y_in)) {
            current_direction++;
            current_direction %= 4;
            Sides++;
            continue;
        }

        int next_index = next_pos[1]*(x_size+1)+next_pos[0];
        char next = map[next_index];

        if (next != cur) {
            current_direction++;
            current_direction %= 4;
            Sides++;
            continue;
        }

        pos[0] = next_pos[0];
        pos[1] = next_pos[1];

        int* side1_vec = directions[( current_direction-1 )%4&4-1];
        int next_side1[2] = {side1_vec[0]+next_pos[0],side1_vec[1]+next_pos[1]};

        bool x_side_in = ( 0<=next_side1[0] ) && ( next_side1[0] < x_size);
        bool y_side_in = (0<=next_side1[1]) && (next_side1[1] < y_size);

        if (x_side_in && y_side_in) {
            int side_index = next_side1[1]*(x_size+1)+next_side1[0];
            char side = map[side_index];
            printf("T!!: %d %d\n",pos[0],pos[1]);
            if (side == cur) {
                current_direction = (current_direction-1)%4&4-1;
                Sides++;
                continue;
            }
        }

    }
    printf("----------[%d]--------\n\n",Sides);
    return Sides;
}

char* imaptostrmap(int* map,int x_size, int y_size){
    int strlen = (x_size+1)*y_size;
    char* strmap = malloc(sizeof(char)*strlen);
    strmap[strlen-1] = '\0';
    int map_index = 0;
    for (int i = 0; i < strlen-1; i++) {
        if (i % (x_size+1) == x_size) {
            strmap[i] = '\n';
            continue;
        }
        strmap[i] = map[map_index]+48;
        map_index++;
    }
    return  strmap;
}

void print_imap(int* map,int x_size, int y_size){
    char* s = imaptostrmap(map, x_size, y_size);
    printf("%s\n",s);
    free(s);
}

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

int FindSides(int* map, int x_size, int y_size){
    int Sides = 0;

    int directions[4][2] = {{0,-1},{1,0},{0,1},{-1,0}};

    for (int current_cd = 0; current_cd < 4; current_cd++) {
        int* check_map = calloc(x_size*y_size,sizeof(int));
        bool side_detected = false;
        int pos[2] = {0,0};
        int max[2] ={x_size,y_size};
        int major_index = current_cd%2;
        for (int i = 0; i < x_size*y_size; i++) {
            int x = pos[0];
            int y = pos[1];
            pos[major_index]++;
            if (pos[major_index]%(max[major_index]+1) == max[major_index]) {
                pos[major_index] = 0;
                pos[1-major_index]++;
            }
            int current_index = y*x_size+x;
            if(map[current_index] == 1){
                side_detected = false;
                continue;
            }
            int* cur_vec = directions[current_cd];
            int side_cord[2] = {x+cur_vec[0],y+cur_vec[1]};
            int side_index = side_cord[1]*x_size+side_cord[0];

            bool x_in = 0<=side_cord[0]  && side_cord[0] < x_size;
            bool y_in = 0<=side_cord[1]  && side_cord[1] < y_size;
            bool curr_side_detection = false;
            if (x_in && y_in) {
                check_map[y*x_size+x] = map[side_index];
                if (map[side_index] == 1){
                    curr_side_detection = true;
                }
            }else {
                check_map[y*x_size+x]= 1;
                curr_side_detection = true;
            }
            if (curr_side_detection && !side_detected) {
                Sides++; 
                side_detected = true;
            }else if (!curr_side_detection) {
                side_detected = false; 
            }

        }
        free(check_map);
    }
    return Sides;
}


int main(){
    FILE* inputs; 
    errno_t err = fopen_s(&inputs,"q12.txt", "rb");

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

    char* check_buf = _strdup(buffer);
    // opertation plan
    //
    // BFS over the grid 
    //  - Main queue
    //  - Next position queue. 
    //
    // Start at 0,0 and read in current plant.
    // Iterate over with BFS 
    // add `next possible` when plant != current plant 
    // No visiting previous positions by marking current with #
    // No appending positions which are already on the queue
    //
    // using a second string buffer with out # to check for sides
    // - this is used for Perimeter calculation
    //
    // [Data agg]
    // Area will be total squares covered
    // Perimeter of a single spot will be 4 - (available next positions)
    //
    // When Queue = 0, stop and add price to sum

    int queue_count = 0;
    int (*queue)[2] = malloc(0);

    int npq_count = 1;
    int npq_pointer = 0;
    int (*npq)[2] = malloc(sizeof(int[2]));
    npq[0][0] = 0;
    npq[0][1] = 0;

    int price = 0;
    for (;npq_pointer!=npq_count;) {
        int queue_pointer = 0;

        queue_count++;
        queue = realloc(queue, sizeof(int[2])*queue_count) ;
        int* next_position = npq[npq_pointer];
        queue[queue_count-1][0] = next_position[0]; 
        queue[queue_count-1][1] = next_position[1]; 

        int target_plant = buffer[next_position[1]*(x_size+1)+next_position[0]];
        if (target_plant == '#') {
            queue_count = 0;
            npq_pointer++;
            continue; 
        }

        int Sides = 0;
        int Area = 0;


        int count =(x_size-1)*y_size;
        int* imap = calloc(count,sizeof(int));
        for (int i = 0; i < count; i++) {
            imap[i] = 1;
        }


        for (;queue_pointer!=queue_count;) {
            //get the queue value
            int *pq = queue[queue_pointer];
            int current_index = pq[1]*(x_size+1)+pq[0];
            int current_plant = buffer[current_index]; // not really needed, its more about the surroundings that are the main thing

            //consume the queue value
            queue_pointer++;

            if (current_plant == '#') {
                continue; 
            }
            buffer[current_index] = '#'; // make as visited
            imap[pq[1]*(x_size-1)+pq[0]] = 0;

            Area++;

            int possible_pos[4][2] = {
                {pq[0],pq[1]-1},
                {pq[0],pq[1]+1},
                {pq[0]-1,pq[1]},
                {pq[0]+1,pq[1]}
            };

            int side_count = 0;
            for (int i = 0; i < 4; i++) {
                int *npos  = possible_pos[i];
                bool is_in_X = 0<=npos[0] && npos[0]<x_size-1;
                bool is_in_Y = 0<=npos[1] && npos[1]<y_size;
                if (is_in_X && is_in_Y) {
                    int sel_npo = npos[1]*(x_size+1)+npos[0];
                    int next_plant = buffer[sel_npo];

                    int side_check = check_buf[sel_npo];
                    if (side_check == target_plant) {
                        side_count++; 
                    }
                    if (next_plant == target_plant) {
                        // make sure not already on the queue;
                        if(!inArrayC(npos,&queue[queue_pointer-1],queue_count-queue_pointer) && next_plant != '#'){
                            queue_count++;
                            queue = realloc(queue, sizeof(int[2])*queue_count) ;
                            queue[queue_count-1][0] = npos[0]; 
                            queue[queue_count-1][1] = npos[1]; 
                        }
                    }else if (next_plant != '#') {
                        if(!inArrayC(npos,npq,npq_count)){
                            npq_count++;
                            npq = realloc(npq,sizeof(int[2])*npq_count);
                            npq[npq_count-1][0] = npos[0];
                            npq[npq_count-1][1] = npos[1];
                        }

                    }
                }
            }
        } 
        Sides = FindSides(imap, x_size-1, y_size);
        free(imap);
        price += Area*Sides;
        queue_count = 0; // rest queue; realloc does the rest
        npq_pointer++;

    }
    printf("%d\n",price);

    free(buffer);
    free(queue);
    free(npq);
    free(check_buf);
}
