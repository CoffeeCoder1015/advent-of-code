#include <math.h>
#include <stdbool.h>
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

// program output manager
int* output;
int output_size = 0;
void init_output(){
    output = malloc(0);
}

void submit_output(int value) {
    output_size++;
    output = realloc(output, sizeof(int) * output_size);
    output[output_size-1] = value;
}

// instruction ptr
int prog_pointer = 0;

//registers
size_t A,B,C;
size_t lit0 = 0;
size_t lit1 = 1;
size_t lit2 = 2;
size_t lit3 = 3;

size_t *operand_map[7] = {&lit0, &lit1, &lit2, &lit3, &A, &B, &C};
void adv(int operand){
    size_t value = *operand_map[operand];
    A >>= value;
    prog_pointer += 2;
}

void bxl(int operand){
    B ^= operand;
    prog_pointer += 2;
}

void bst(int operand){
    size_t value = *operand_map[operand];
    B = value%8; 
    prog_pointer += 2;
}

void jnz(int operand){
    if (A == 0) {
      prog_pointer += 2;
      return;
    }
    prog_pointer = operand;
}

void bxc(int operand){
    B ^= C;
    prog_pointer += 2;
}

void out(int operand){
    size_t value = *operand_map[operand];
    int mod = value%8;
    submit_output(mod);
    prog_pointer += 2;
}

void bdv(int operand){
    size_t value = *operand_map[operand];
    B = A>>value;
    prog_pointer += 2;
}

void cdv(int operand){
    size_t value = *operand_map[operand];
    C = A>>value;
    prog_pointer += 2;
}

void (*opcode_map[8])(int) = {adv,bxl,bst,jnz,bxc,out,bdv,cdv};
int main(){
    FILE* inputs;
    errno_t err = fopen_s(&inputs,"q17.txt", "r");
    char buffer[1024];
    // Reg A
    fgets(buffer,1024,inputs);
    A = atoll(strchr(buffer, ':')+1);
    buffer[0] = 0;
    // Reg B
    fgets(buffer,1024,inputs);
    B = atoll(strchr(buffer, ':')+1);
    buffer[0] = 0;
    // Reg C
    fgets(buffer,1024,inputs);
    C = atoll(strchr(buffer, ':')+1);
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
        program[i] = atoll(raw_program[i]);
        free(raw_program[i]);
    }
    free(raw_program);
    
    
    init_output();
    for (; prog_pointer<size;) {
        int op = program[prog_pointer];
        int operand = program[prog_pointer+1];
        opcode_map[op](operand);
    }
    free(program);
    for (int i = 0; i < output_size; i++) {
        printf("%d",output[i]);
        if (i+1 < output_size) {
            printf(",");
        }
    }
    printf("\n");
    free(output);
}