#include <stddef.h>
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

//registers
int A,B,C;

int main(){
    FILE* inputs;
    errno_t err = fopen_s(&inputs,"test.txt", "r");
    char buffer[1024];
    // Reg A
    fgets(buffer,1024,inputs);
    A = atoi(strchr(buffer, ':')+1);
    buffer[0] = 0;
    // Reg B
    fgets(buffer,1024,inputs);
    B = atoi(strchr(buffer, ':')+1);
    buffer[0] = 0;
    // Reg C
    fgets(buffer,1024,inputs);
    C = atoi(strchr(buffer, ':')+1);
    buffer[0] = 0;
    
    fseek(inputs, 2 , SEEK_CUR);

    fgets(buffer,1024,inputs);
    char* prog_start = strchr(buffer, ':') + 2;
    char** raw_program = split(prog_start, ',');
    int size = 0;
    for (int i = 0; raw_program[i]!=0;i++) {
        size++;
    }
    
    int* program = malloc(sizeof(int)*size);
    for (int i = 0; i<size;i++) {
        program[i] = atoi(raw_program[i]);
        free(raw_program[i]);
    }
    free(raw_program);
    free(program);
    
}
