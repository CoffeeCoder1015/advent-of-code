#include <math.h>
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
int lit0 = 0;
int lit1 = 1;
int lit2 = 2;
int lit3 = 3;

int *operand_map[7] = {&lit0, &lit1, &lit2, &lit3, &A, &B, &C};
void adv(int value){
    double pow_result = pow(2, value);
    A /= pow_result;
}

void (*opcode[8])(int) = {adv};
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
    
    
    for (int prog_pointer = 0; prog_pointer<size;) {
        int op = program[prog_pointer];
        int operand = program[prog_pointer+1];
        int value = *operand_map[operand];
    }

    
    free(program);
}
