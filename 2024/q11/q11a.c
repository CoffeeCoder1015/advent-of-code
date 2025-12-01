#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** split(char string[], char delim){
    size_t delim_count = 0;
    char* last_delim = 0;
    char *pch=strchr(string,delim);
    while (pch!=NULL) {
        delim_count++;
        last_delim = pch;
        pch=strchr(pch+1,delim);
    }

    /* add null byte */
    delim_count+=2;

    char** strarray = malloc(sizeof(char*)*delim_count);

    char* next_tok = NULL;
    char delim_str[2] = {delim, '\0'};
    char* token = strtok_s(string, delim_str, &next_tok);
    int i = 0;
    while (token != NULL) {
        char* s  =  _strdup(token);
        strarray[i] = s;
        // printf("%d %s\n",i,s);
        token = strtok_s(NULL,delim_str,&next_tok);
        i++;
    }
    strarray[i] = 0;
    return  strarray;
}

int main(){
    FILE* inputs; 
    errno_t err = fopen_s(&inputs,"q11.txt", "r");
    char buffer[1024];
    fgets(buffer, 1024, inputs);
    fclose(inputs);
    char* next_line = strchr(buffer, '\n');
    *next_line = '\0';

    int input_count = 0;
    char** split_input = split(buffer, ' ');
    for (int i = 0; split_input[i] != 0; i++) {
        input_count = i+1; 
    }
    int working_buffer_count = input_count;
    unsigned long long* working_buffer = malloc(sizeof(unsigned long long)*working_buffer_count);
    for (int i = 0; i < working_buffer_count; i++) {
        // unsigned long long to_num = atoll(split_input[i]);
        unsigned long long to_num = strtoll(split_input[i], NULL, 10);
        working_buffer[i] = to_num;
    }
    //buffer swap:
    //working buffer
    //make next buffer
    //
    //free working buffer
    //working buffer pointer = next buffer
    //make next buffer
    
    int next_buffer_count = 0;
    unsigned long long* next_buffer = malloc(0);
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < working_buffer_count; j++) {
            unsigned long long current = working_buffer[j];
            if(current == 0){
                next_buffer_count++;
                next_buffer = realloc(next_buffer, sizeof(unsigned long long)*next_buffer_count);
                next_buffer[next_buffer_count-1] = 1;
            }else{
                double digit_count = floor(log10(current))+1;
                if ((int)digit_count % 2 == 0) {
                    next_buffer_count+=2;
                    next_buffer = realloc(next_buffer, sizeof(unsigned long long)*next_buffer_count);
                    double half_dl = digit_count/2;
                    double shift = pow(10, half_dl);
                    unsigned long long split_left = current/shift;
                    unsigned long long split_right = current-split_left*shift;
                    next_buffer[next_buffer_count-2] = split_left;
                    next_buffer[next_buffer_count-1] = split_right;
                }else {
                    next_buffer_count++;
                    next_buffer = realloc(next_buffer, sizeof(unsigned long long)*next_buffer_count);
                    next_buffer[next_buffer_count-1] = current*2024;
                }
            }
        }

        unsigned long long* old_wb_address = working_buffer;

        working_buffer = next_buffer;
        working_buffer_count = next_buffer_count;

        next_buffer_count = 0;
        next_buffer = old_wb_address;
    }
    printf("%d\n",working_buffer_count);
    free(working_buffer);
    free(next_buffer);
}
