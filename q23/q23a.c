#include <stdbool.h>
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
        if (c1[0] > c2[0]) {
            char* t = c2;
            c2 = c1;
            c1 = t;
        }else if (c1[0] == c2[0]) {
            if (c1[1] > c2[1]) {
                char* t = c2;
                c2 = c1;
                c1 = t;
            }
        }

        printf("%s %s\n",c1,c2);
    }
    fclose(input);
}
