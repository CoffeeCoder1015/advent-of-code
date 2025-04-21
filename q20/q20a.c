#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
int main(){
    FILE* input;
    fopen_s(&input, "test.txt", "rb");

    fseek(input, 0, SEEK_END);
    size_t file_size = ftell(input);
    rewind(input);

    char* map = malloc(file_size);
    fread(map, sizeof(char) , file_size, input);
    map[file_size-1] = '\0';
    fclose(input);

    printf("%s\n",map);

    free(map);
}
