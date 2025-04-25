#include <stdio.h>
#include <string.h>
int main(){
    FILE* input;
    fopen_s(&input, "test.txt", "r");

    for (;;) {
        char buffer[1024];
        char* end = fgets(buffer, 1024 , input);
        if (end == NULL) {
            break; 
        }
        int n = strlen(buffer);
        buffer[--n] = '\0';
        printf("%s\n",buffer);
    }
}
