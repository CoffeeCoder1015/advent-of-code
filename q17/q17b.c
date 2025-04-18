#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#ifdef _WIN32
#  include <fcntl.h>
#  include <io.h>
#endif

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

// #define _FuncLog

size_t *operand_map[7] = {&lit0, &lit1, &lit2, &lit3, &A, &B, &C};
void adv(int operand){
    size_t value = *operand_map[operand];
    A >>= value;
#ifdef _FuncLog
    wprintf(L"\x1b[38;2;48;227;224madv() [Resolved: %zu] [From: %d] \x1b[0m\n",value,operand);
#endif /* ifdef MACRO */
    prog_pointer += 2;
}

void bxl(int operand){
    B ^= operand;
#ifdef _FuncLog
    wprintf(L"\x1b[38;2;50;168;50mbxl() [Operand: %d]\x1b[0m\n",operand);
#endif /* ifdef MACRO */
    prog_pointer += 2;
}

void bst(int operand){
    size_t value = *operand_map[operand];
    B = value%8; 
#ifdef _FuncLog
    wprintf(L"\x1b[38;2;194;255;223mbst() [Full: %zu] [From: %d]\x1b[0m\n",value,operand);
#endif /* ifdef MACRO */
    prog_pointer += 2;
}

void jnz(int operand){
    if (A == 0) {
      prog_pointer += 2;
      return;
    }
#ifdef _FuncLog
    wprintf(L"\x1b[38;2;232;63;193mjnz() jump to %d\x1b[0m\n",operand);
#endif /* ifdef MACRO */
    prog_pointer = operand;
}

void bxc(int operand){
#ifdef _FuncLog
    wprintf(L"\x1b[38;2;177;209;50mbxc() B = %zu XOR %zu\x1b[0m\n",B,C);
#endif /* ifdef MACRO */
    B ^= C;
    prog_pointer += 2;
}

void out(int operand){
    size_t value = *operand_map[operand];
    int mod = value%8;
#ifdef _FuncLog
    wprintf(L"\x1b[38;2;227;89;48m[Output: %d] [Full: %zu] [From: %d]\x1b[0m\n", mod, value, operand);
#endif /* ifdef MACRO */
    submit_output(mod);
    prog_pointer += 2;
}

void bdv(int operand){
    size_t value = *operand_map[operand];
    B = A>>value;
#ifdef _FuncLog
    wprintf(L"\x1b[38;2;59;78;138mbdv() [Resolved: %zu] [From: %d] \x1b[0m\n",value,operand);
#endif /* ifdef MACRO */
    prog_pointer += 2;
}

void cdv(int operand){
    size_t value = *operand_map[operand];
    C = A>>value;
#ifdef _FuncLog
    wprintf(L"\x1b[38;2;116;78;194mcdv() [Resolved: %zu] [From: %d] \x1b[0m\n",value,operand);
#endif /* ifdef MACRO */
    prog_pointer += 2;
}

void (*opcode_map[8])(int) = {adv,bxl,bst,jnz,bxc,out,bdv,cdv};

/* Trace functions*/

void recordRegister(size_t oldA, size_t oldB, size_t oldC){
    wprintf(L"[A:%zu B:%zu C:%zu] -> [A:%zu B:%zu C:%zu]\n",oldA,oldB,oldC,A,B,C);
}
void recordJump(int old, int new, char* prog_str){
    int n = strlen(prog_str);

    wchar_t end = L'↓';
    wchar_t start1 = L'⌉';
    wchar_t start2 = L'⌈';
    wchar_t line = L'¯';

    int line_n =  abs(new-old)*2-1;
    wchar_t* line_r = malloc(line_n*sizeof(wchar_t)+1);
    for (int i = 0; i < line_n; i++) {
        line_r[i]  = line;
    }
    line_r[line_n] = L'\0';

    wchar_t buffer[100];
    if (new < old) {
        swprintf(buffer,100, L"%*lc%ls%lc\n", 2*new+1,end,line_r,start1); 
    }else{
        swprintf(buffer,100, L"%*lc%ls%lc\n", 2*old+1, start2,line_r,end); 
    }
    wprintf(L"%ls",buffer);
    wprintf(L"%s\n",prog_str);
}

/* Reset state */
void resetState(){
    prog_pointer = 0;
    A = 0;
    B = 0;
    C = 0;
    free(output);
    output = malloc(0);
    output_size = 0;
}

void printOutput(){
    for (int i = 0; i < output_size; i++) {
        wprintf(L"%zu",output[i]);
        if (i+1 < output_size) {
            wprintf(L",");
        }
    }
    wprintf(L"\n");
}


void runProgram_Opt(int* program, int size){
    for (; prog_pointer<size;) {
        int op = program[prog_pointer];
        int operand = program[prog_pointer+1];
        opcode_map[op](operand);
        if (op == 5) {
            break; 
        }
    }
}
void runProgram(int* program, int size){
    for (; prog_pointer<size;) {
        int op = program[prog_pointer];
        int operand = program[prog_pointer+1];

        int old_ptr = prog_pointer;
        size_t oldA = A;
        size_t oldB = B;
        size_t oldC = C;
        opcode_map[op](operand);
        // recordRegister(oldA,oldB,oldC);
        // recordJump(old_ptr, prog_pointer, raw_copy);
    }
}



int main(){
    setlocale(LC_ALL, "");

#ifdef _WIN32
    _setmode(_fileno(stdout), _O_U16TEXT);
#endif
    FILE* inputs;
    errno_t err = fopen_s(&inputs,"test.txt", "r");
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

    char* raw_copy = _strdup(prog_start);

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

        int old_ptr = prog_pointer;
        size_t oldA = A;
        size_t oldB = B;
        size_t oldC = C;
        opcode_map[op](operand);
        // recordRegister(oldA,oldB,oldC);
        // recordJump(old_ptr, prog_pointer, raw_copy);
    }
    printOutput();

    free(program);
    free(output);
}
