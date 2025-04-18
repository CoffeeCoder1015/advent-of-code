#include <stdio.h>
#include <stdlib.h>

int main(){
    FILE* input;
    fopen_s(&input,"test.txt","rb");

    fseek(input, 0, SEEK_END);
    size_t read_count = ftell(input);
    rewind(input);

    char* raw_coords = malloc(read_count+1);
    raw_coords[read_count] = '\0';
    fread(raw_coords, sizeof(char), read_count, input);
    fclose(input);


    free(raw_coords);
}
