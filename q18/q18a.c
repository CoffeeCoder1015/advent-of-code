#include <stdio.h>
#include <stdlib.h>

#define TESTING
#ifdef TESTING
    #define SQR_SIZE 7
    #define INPUT "test.txt"
#else
    #define SQR_SIZE 71
    #define INPUT "q18.txt"
#endif

int square_size = SQR_SIZE;

int main(){
    FILE* input;
    fopen_s(&input,INPUT,"rb");

    fseek(input, 0, SEEK_END);
    size_t read_count = ftell(input);
    rewind(input);

    char* raw_coords = malloc(read_count+1);
    raw_coords[read_count] = '\0';
    fread(raw_coords, sizeof(char), read_count, input);
    fclose(input);


    free(raw_coords);
}
