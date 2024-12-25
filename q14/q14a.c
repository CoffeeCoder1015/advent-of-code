#include <stdio.h>
int main(){
    // int real_dim[2] = {101,103};
    FILE* inputs;
    int test_dim[2] = {11,7};
    errno_t err = fopen_s(&inputs,"test.txt", "r");
    for (;;) {
        char buffer[1024];
        char* r = fgets(buffer, 1024, inputs);
        if (r == NULL) {
            break; 
        }
        //parsing p 
        //parsing v 
    }
    fclose(inputs);
}
