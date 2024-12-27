#include <corecrt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
int main(){
    FILE* inputs;
    errno_t err = fopen_s(&inputs , "test.txt", "rb");
    fseek(inputs, 0, SEEK_END);
    size_t read_count = ftell(inputs);
    rewind(inputs);
    char* buffer = malloc(sizeof(char)*(read_count+1));
    fread(buffer,sizeof(char),read_count,inputs);
    buffer[read_count] = '\0';


    free(buffer);
}
