#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
    FILE* inputs; 
    errno_t err = fopen_s(&inputs,"q9.txt", "r");

    fseek(inputs, 0, SEEK_END);
    size_t character_count = ftell(inputs);
    rewind(inputs);

    size_t alloc_size = sizeof(char)*(character_count);
    char* buffer = malloc(alloc_size);
    fread(buffer,sizeof(char),character_count,inputs);

    fclose(inputs);
    char* linebreak = strchr(buffer, '\n');
    *linebreak = '\0';

    int amount_of_files = 0;
    int (*files)[2] = malloc(0);
    // file struct
    // file is a int[2],[0] is position, [1] is size
    int amount_of_free_space = 0;
    int (*free_space)[2] = malloc(0);
    // free_space struct
    // free_space is a int[2], [0] is positon, [1] is size

    int pointer_position = 0;
    for (int i = 0; buffer[i] != '\0'; i++) {
        int current = buffer[i]-48;
        if (i % 2 == 0) {
            int fileID = i/2;
            amount_of_files++;
            files = realloc(files,sizeof(int[2])*amount_of_files);
            files[amount_of_files-1][0] = pointer_position;
            files[amount_of_files-1][1] = current;
        }else{
            amount_of_free_space++;
            free_space = realloc(free_space,sizeof(int[2])*amount_of_free_space);
            free_space[amount_of_free_space-1][0] = pointer_position;
            free_space[amount_of_free_space-1][1] = current;
        }
        pointer_position+=current;
    }

    int free_space_start = 0;
    for (int i = amount_of_files-1; i >= 0; i--) {
        int* file = files[i];
        int position = file[0];
        int size = file[1];
        if (free_space[free_space_start][0] > position) {
            break; 
        }
        if (free_space[free_space_start][1] == 0) {
            free_space_start++; 
        }
        for (int j = free_space_start; j < amount_of_free_space; j++) {
            int* fs = free_space[j];
            int fs_positon = fs[0];
            int space = fs[1];
            if(size <= space){
                files[i][0] = fs[0];
                free_space[j][1] = space-size;
                free_space[j][0]+=size;
                break;
            }else if (fs_positon > position) {
                break; 
            }

        }
    }

    long long checksum = 0;
    for (int i = 0; i < amount_of_files; i++) {
        int* file = files[i];
        long long id = i; 
        long long position = file[0];
        long long size = file[1];
        checksum+= id*size*(2*position+size-1)/2;
        // for (int j = 0; j < size; j++) {
        //     checksum+=id*(j+position);
        // }
    }
    printf("%lld\n",checksum);

    free(files);
    free(free_space);
    free(buffer);
}
