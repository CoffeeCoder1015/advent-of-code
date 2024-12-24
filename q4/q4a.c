#include <stdlib.h>
#include <corecrt.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>


// char** split(char string[], char delim){
//     size_t delim_count = 0;
//     char* last_delim = 0;
//     char *pch=strchr(string,delim);
//     while (pch!=NULL) {
//         delim_count++;
//         last_delim = pch;
//         pch=strchr(pch+1,delim);
//     }
//
//     /* add null byte */
//     delim_count+=2;
//
//     char** strarray = malloc(sizeof(char*)*delim_count);
//
//     char* next_tok = NULL;
//     char delim_str[2] = {delim, '\0'};
//     char* token = strtok_s(string, delim_str, &next_tok);
//     int i = 0;
//     while (token != NULL) {
//         char* s  =  _strdup(token);
//         strarray[i] = s;
//         // printf("%d %s\n",i,s);
//         token = strtok_s(NULL,delim_str,&next_tok);
//         i++;
//     }
//     strarray[i] = 0;
//     return  strarray;
// }

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

    //     6   7   8
    //      S  S  S
    //       A A A
    //        MMM
    //    5 SAMXMAS 1
    //        MMM
    //       A A A
    //      S  S  S
    //     4   3   2
    size_t total_XMAS_count = 0;
    for (size_t y = 0; y<y_size; y++) {
        for (size_t x = 0; x<x_size; x++) {
            char focus = buffer[y*( x_size+1 )+x];
            if(focus != 'X'){
                continue;
            }
            int dir[8][2] = {{3,0},{3,3},{0,3},{-3,3},{-3,0},{-3,-3},{0,-3},{3,-3}};
            for (size_t d = 0; d < 8; d++) {
                int* current_offset = dir[d];
                int signX = (current_offset[0] != 0) | (current_offset[0] >> 31);
                int signY = (current_offset[1] != 0) | (current_offset[1] >> 31);
                // printf("%d %d ",current_offset[0],current_offset[1]);
                // printf("%d %d\n",signX,signY);
                int endX = current_offset[0]+x;
                int endY = current_offset[1]+y;
                bool x_in = ( 0<=endX ) && ( endX < x_size);
                bool y_in = (0<=endY) && (endY < y_size);
                if(x_in && y_in){ 
                    char check[4];
                    check[3] = 0;
                    for (size_t i = 0; i < 3; i++) {
                        int offset = i+1;
                        int y_offset = signY*offset;
                        int x_offset = signX*offset;
                        check[i] = buffer[( y + y_offset )*(x_size+1)+( x + x_offset)];
                    }
                    if(strcmp(check, "MAS")==0){
                        // printf("%s\n",check);
                        total_XMAS_count++;
                    }
                }
            }
        }
    }
    printf("%d\n",total_XMAS_count);
    free(buffer);
}
