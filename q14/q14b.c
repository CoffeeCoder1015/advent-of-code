#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    int x;
    int y;
} position;

double modulo(double x, double y){
    //why does the C modulo operator seem to only work on positive numbers, I will never know. . . .
    return x-floor(x/y)*y;
}

position simulate_one(int dim[2], int p[2], int v[2]){
    int time = 100;
    int raw_final_positions[2] = {p[0]+time*v[0],p[1]+time*v[1]};
    int x_wrap = modulo(raw_final_positions[0], dim[0]);
    int y_wrap = modulo(raw_final_positions[1], dim[1]);
    return (position){x_wrap,y_wrap};
}

int main(){
    FILE* inputs;
    int dim[2] = {101,103};
    // int dim[2] = {11,7};
    errno_t err = fopen_s(&inputs,"q14.txt", "r");
    int quad1 = 0;
    int quad2 = 0;
    int quad3 = 0;
    int quad4 = 0;
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
        int p[2] = {p_x,p_y};
        // v string --> number values
        char* v_seperator = strchr(v_start, ',');
        *v_seperator = '\0';
        int v_x = atoi(v_start);
        int v_y = atoi(v_seperator+1);
        int v[2] = {v_x,v_y};

        position final_pos = simulate_one(dim, p, v);

        int x_diff = final_pos.x - ( dim[0] / 2  );
        int y_diff = final_pos.y - ( dim[1] / 2  );
        int x_state = ( x_diff >> 31 ) | (x_diff != 0);
        int y_state = ( y_diff >> 31 ) | (y_diff != 0);
        if ((x_state != 0) && (y_state != 0)) {
            if (x_state == -1 && y_state == -1) {
                quad1 += 1;         
            } else if (x_state == -1 && y_state == 1){
                quad2 += 1;
            } else if (x_state == 1 && y_state == -1) {
                quad3 += 1;
            } else {
                quad4 += 1;
            }
        }
    }
    printf("%d\n",quad1*quad2*quad3*quad4);
    fclose(inputs);
}
