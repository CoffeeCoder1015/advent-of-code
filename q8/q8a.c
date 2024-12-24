#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct{
    int n;
    int (*coord_array)[2];
} array_wrap;

int compare_func(const void *a, const void *b)
{
    int i_a = *((int *)a);
    int i_b = *((int *)b);

    if (i_a < i_b)
        return -1;
    else if (i_a == i_b)
        return 0;
    else
        return 1;
}

int* get_keys(array_wrap** map,size_t n){
    size_t key_count = 0;
    int* keys = malloc(0);
    for (size_t i = 0; i < n; i++) {
        array_wrap* current_key =  map[i];
        if(current_key != 0){
            //append to keys
            key_count++;
            int* keys_tmp = realloc(keys, sizeof(int)*( key_count+1) );
            if(keys_tmp == NULL){
                printf("get keys failed to alloc new space\n");
                exit(1);
            }
            keys = keys_tmp;
            keys[key_count-1] = i;
        }
    }
    keys[key_count] = 0;
    return keys;
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
    //* NOTE: temp *//
    // char tmp_f[4096];
    // tmp_f[0] = 0;

    FILE* inputs;
    errno_t err = fopen_s(&inputs,"q8.txt", "r");

    int y_size = 0;
    int x_size = 0;

    int map_count=0;
    array_wrap** map = malloc(0);


    for(;;){
        char buffer[1024];
        char* r = fgets(buffer, 1024, inputs);
        if (r == NULL) {
            break; 
        }
        // strcat_s(tmp_f, sizeof(char)*4096, buffer);
        x_size = strchr(buffer, '\n')-buffer;
        int i;
        for (i = 0; buffer[i] != '\n'; i++) {
            if (buffer[i] != '.') {
                int key = buffer[i];
                if (key >= map_count) {
                    int new_count = key+1;             
                    array_wrap** tmp_map = realloc(map, sizeof(array_wrap*)*new_count);
                    if (tmp_map == NULL) {
                        printf("Extending map failed!\n");
                        exit(1);
                    }
                    map = tmp_map;
                    for (int i = map_count; i < new_count; i++) {
                        map[i] = 0;
                    }
                    map_count = new_count;
                }
                if (map[key] == 0) {
                    array_wrap* new_freqency = malloc(sizeof(array_wrap));
                    if (new_freqency == NULL) {
                        printf("failed to make new new_freqency entry\b");
                        exit(1);
                    }
                    new_freqency->n = 1;
                    new_freqency->coord_array = malloc(sizeof(int[2]));
                    map[key] = new_freqency;
                }else {
                    int len = ++map[key]->n;
                    int ( *array )[2] = map[key]->coord_array;
                    int ( *tmp_coords )[2] = realloc(array, sizeof(int[2])*len);
                    if (tmp_coords == NULL) {
                        printf("Extending coord in map failed!\n");
                        exit(1);
                    }
                    map[key]->coord_array = tmp_coords ;
                }

                int len = map[key]->n;
                int x = i;
                int y = y_size;
                map[key]->coord_array[len-1][0] = x ;
                map[key]->coord_array[len-1][1] = y ;
            }
        }
        y_size++;
    }
    fclose(inputs);

    int* keys = get_keys(map, map_count);
    size_t keys_n = 0;
    for (int i = 0; keys[i] != 0; i++) {
        keys_n = i;
    }

    int occupied_count = 0;
    int* occupied = malloc(0);

    int unique_count = 0;
    for (int i = 0; i < keys_n+1; i++) {
        int key = keys[i];
        array_wrap value = *map[key];
        int len = value.n;
        int (*arr)[2] = value.coord_array;
        // printf("Key:%c | %d ",key,key);
        for (int j = 0; j < len; j++) {
            int* p1 = arr[j];
            for (int k = j+1; k < len; k++) {
                int* p2 = arr[k];
                int vec[2] = {p1[0]-p2[0],p1[1]-p2[1]};
                int r1[2] = {p1[0]+vec[0],p1[1]+vec[1]};
                int r2[2] = {p2[0]-vec[0],p2[1]-vec[1]};

                bool x_in_r1 = 0<=r1[0] && r1[0]<x_size;
                bool y_in_r1 = 0<=r1[1] && r1[1]<y_size;
                int converetd_r1 = r1[1]*( x_size+1 )+r1[0];
                bool not_occ_r1 = !inArray(converetd_r1, occupied, occupied_count);
                if (x_in_r1 && y_in_r1) {
                    if(not_occ_r1){
                        unique_count++;
                        // tmp_f[converetd_r1] = '#';
                        occupied_count++;
                        occupied=realloc(occupied, sizeof(int)*occupied_count); 
                        occupied[occupied_count-1]  = r1[1]*( x_size+1 )+r1[0];
                        // printf("(%d,%d) ",r1[0],r1[1]);
                    }
                }

                bool x_in_r2 = 0<=r2[0] && r2[0]<x_size;
                bool y_in_r2 = 0<=r2[1] && r2[1]<y_size;
                int converetd_r2 = r2[1]*( x_size+1 )+r2[0];
                bool not_occ_r2 = !inArray(converetd_r2, occupied, occupied_count);
                if (x_in_r2 && y_in_r2) {
                    if(not_occ_r2){
                        // tmp_f[converetd_r2] = '#';
                        unique_count++;
                        occupied_count++;
                        occupied=realloc(occupied, sizeof(int)*occupied_count); 
                        occupied[occupied_count-1]  = r2[1]*( x_size+1 )+r2[0];
                        // printf("(%d,%d) ",r2[0],r2[1]);
                    }
                }
            }
        }
        // printf("\n");
        free(map[key]->coord_array);
        free(map[key]);
    }
    // printf("\n%s\n",tmp_f);
    printf("%d\n",unique_count);
    free(occupied);
    free(map);
    free(keys);
}
