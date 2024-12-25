#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int* simulate_one(int dim[2], int p[2], int v[2]){

}

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
        char* p_start = strchr(buffer, '=')+1;
        char* p_end = strchr(p_start, ' ');
        //parsing v 
        char* v_start = strchr(p_end, '=')+1;
        char* v_end = strchr(v_start, '\n');
        //seperate p and v from the rest of the string
        *p_end = '\0';
        *v_end = '\0';
        // p string --> number values
        char* p_seperator = strchr(p_start, ',');
        *p_seperator = '\0';
        int p_x = atoi(p_start);
        int p_y = atoi(p_seperator+1);
        // v string --> number values
        char* v_seperator = strchr(v_start, ',');
        *v_seperator = '\0';
        int v_x = atoi(v_start);
        int v_y = atoi(v_seperator+1);
        printf("%d %d %d %d\n",p_x,p_y,v_x,v_y);
    }
    fclose(inputs);
}
