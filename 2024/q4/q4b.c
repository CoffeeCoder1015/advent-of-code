#include <stdlib.h>
#include <corecrt.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>


int main(){
    FILE* inputs; 
    errno_t err = fopen_s(&inputs,"q4.txt", "r");

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

    // M.S
    // .A.
    // M.S
    size_t total_XMAS_count = 0;
    for (size_t y = 0; y<y_size; y++) {
        for (size_t x = 0; x<x_size; x++) {
            char focus = buffer[y*( x_size+1 )+x];
            if(focus != 'A'){
                continue;
            }
            
            //check for valid on X,Y
            //i.e check if A has enough surrounding space rather than check indivisual branches
            bool x_in = ( 1<=x ) && ( x < x_size-1);
            bool y_in = (1<=y) && (y < y_size-1);
            if(x_in == false && y_in == false){
                continue;
            }

            int diags[2][2] = {{1,1},{1,-1}};
            size_t valid_diags = 0;
            for (size_t d = 0; d < 2; d++) {
                int* current_offset = diags[d];
                char check[3];
                check[2] = 0;
                for (size_t p = 0 ; p < 2; p++) {
                    int multiplier = p * -1 | 1;
                    int x_offset = multiplier*current_offset[0];
                    int y_offset = multiplier*current_offset[1];
                    check[p] = buffer[( y + y_offset )*(x_size+1)+( x + x_offset)];
                }
                if(strcmp(check, "MS")==0 || strcmp(check, "SM") == 0){
                    valid_diags++;
                }
            }
            if(valid_diags == 2){
                total_XMAS_count++;
            }
        }
    }
    printf("%d\n",total_XMAS_count);
    free(buffer);
}
