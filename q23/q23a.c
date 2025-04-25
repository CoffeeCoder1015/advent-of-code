#include <stddef.h>
#include <stdio.h>
#include <string.h>
int main(){
    FILE* input;
    fopen_s(&input, "test.txt", "rb");
    for (;;) {
        char buffer[1024];
        char* end = fgets(buffer,1024,input);
        if (end == NULL) {
            break; 
        }
        int n = strlen(buffer);
        buffer[--n] = '\0';
        buffer[2] = '\0';
        char* c1 = &buffer[0];
        char* c2 = &buffer[3];
        printf("%s %s\n",c1,c2);
    }
    fclose(input);
}
