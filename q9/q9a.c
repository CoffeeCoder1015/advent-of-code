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
    int* files = malloc(0);
    // file struct
    // file is a int[2], [0] is size, [1] is ID
    for (int i = 0; buffer[i] != '\0'; i++) {
        int current = buffer[i]-48;
        if (i % 2 == 0) {
            amount_of_files++;
            files = realloc(files,sizeof(int)*amount_of_files);
            files[amount_of_files-1] = current;
        }
    }

    int final_files_amount = 0;
    int* final_files = malloc(0);
    int forward = 0;
    int backward = amount_of_files-1;
    for (int i = 0; buffer[i] != '\0'; i++) {
        int current = buffer[i]-48;
        if (i % 2 == 0) {
            int n = files[forward];
            int id = forward;
            if (n == 0) {
                break;
            }
            final_files_amount+=n; 
            final_files = realloc(final_files, sizeof(int)*final_files_amount);
            for (int j = -n;j < 0; j++) {
                final_files[j+final_files_amount] = id;
            }
            files[forward] = 0;
            forward++;
        }else {
            for (int j = 0; j < current; j++) {
                files[backward]--;
                int n = files[backward];
                if (n == -1) {
                    break; 
                }
                int id = backward;
                final_files_amount++; 
                final_files = realloc(final_files, sizeof(int)*final_files_amount);
                final_files[final_files_amount-1] = id;
                if (n == 0) {
                    backward--;
                }
            } 
        }
    }
    long long checksum = 0;
    for (int i = 0; i < final_files_amount; i++) {
        checksum += i*final_files[i];
    }
    printf("%lld",checksum);

    free(final_files);
    free(files);
    free(buffer);
}
