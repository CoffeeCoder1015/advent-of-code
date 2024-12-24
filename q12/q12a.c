#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void print_coord_array(int (*cords)[2], int n){
    for (int i =0; i<n; i++) {
        printf("(%d,%d) ",cords[i][0],cords[i][1]) ;
    }
    printf("\n");
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
int main(){
    FILE* inputs; 
    // errno_t err = fopen_s(&inputs,"q12.txt", "rb");
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
        int Perimeter = 0;
        int Area = 0;
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

            Area++;
            // print_coord_array(queue, queue_count);
            // // print_coord_array(&npq[npq_pointer],npq_count-npq_pointer);
            // printf("%s\n[%d/%d => %c  %d/%d][%d]\n",buffer,queue_pointer,queue_count,target_plant,npq_pointer,npq_count,Area);

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
                    // printf("%d %d from \n",npos[0],npos[1]);
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
            Perimeter += 4-side_count;
        } 
        price += Area*Perimeter;
        // printf("%d %d %c\n",Area,Perimeter,target_plant);
        queue_count = 0; // rest queue; realloc does the rest
        npq_pointer++;
        
    }
    printf("%d\n",price);

    free(buffer);
    free(queue);
    free(npq);
    free(check_buf);
}
