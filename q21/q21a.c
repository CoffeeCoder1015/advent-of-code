#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
    FILE* input;
    fopen_s(&input, "test.txt", "rb");

    for (;;) {
        char buffer[1024];
        char* end = fgets(buffer, 1024 , input);
        if (end == NULL) {
            break; 
        }

        printf("%s",buffer);
        char* termination = strchr(buffer,'A');
        *termination = '\0';
        int interger_component = atoi(buffer);
        printf("%d\n",interger_component);
    }
}
