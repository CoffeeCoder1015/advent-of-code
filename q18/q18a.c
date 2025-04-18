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
char gridAtlas[] = ".#O";

void printGrid(int* grid){
    for (int i = 0; i < square_size*square_size; i++) {
        printf("%c",gridAtlas[grid[i]]);
        if ((i+1)%square_size == 0) {
            printf("\n");
        }
    }
}

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

    int* memory = calloc(square_size*square_size, sizeof(int));


    free(raw_coords);
}
