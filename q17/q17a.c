#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    FILE* inputs;
    errno_t err = fopen_s(&inputs,"test.txt", "r");
    char buffer[1024];
    // Reg A
    fgets(buffer,1024,inputs);
    int A = atoi(strchr(buffer, ':')+1);
    buffer[0] = 0;
    // Reg B
    fgets(buffer,1024,inputs);
    int B = atoi(strchr(buffer, ':')+1);
    buffer[0] = 0;
    // Reg C
    fgets(buffer,1024,inputs);
    int C = atoi(strchr(buffer, ':')+1);
    buffer[0] = 0;
    
    fseek(inputs, 2 , SEEK_CUR);

    fgets(buffer,1024,inputs);
    char* prog_start = strchr(buffer, ':') + 2;
    printf("%s\n",prog_start);
}
