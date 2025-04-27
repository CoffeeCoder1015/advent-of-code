#include <stdio.h>
int main(){
    FILE* inputs;
    fopen_s(&inputs, "test.txt", "r");
    char buffer[1024];
    for (;;) {
        char* end = fgets(buffer, 1024, inputs);
        if (buffer[0] == '\n') {
            break;;
        }
    }

    for (;;) {
        char* end = fgets(buffer, 1024, inputs);
        if (end == NULL) {
            break; 
        }
    }
    fclose(inputs);
}
