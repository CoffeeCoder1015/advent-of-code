// Another solution method using CRT  --> what it is https://crypto.stanford.edu/pbc/notes/numbertheory/crt.html
#include <math.h>
#include <stdbool.h>
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

char* new_str_map(int x_size, int y_size){
    int strlen = (x_size+1)*y_size;
    char* strmap = malloc(sizeof(char)*strlen);
    strmap[strlen-1] = '\0';
    int map_index = 0;
    for (int i = 0; i < strlen-1; i++) {
        if (i % (x_size+1) == x_size) {
            strmap[i] = '\n';
            continue;
        }
        strmap[i] = ' ';
        map_index++;
    }
    return  strmap;
}

position simulate_one(int dim[2], int p[2], int v[2], int time){
    int raw_final_positions[2] = {p[0]+time*v[0],p[1]+time*v[1]};
    int x_wrap = modulo(raw_final_positions[0], dim[0]);
    int y_wrap = modulo(raw_final_positions[1], dim[1]);
    return (position){x_wrap,y_wrap};
}

typedef struct{
    int p[2];
    int v[2];
} robot_data;

typedef struct{
    double mean;
    double std;
} stats_pack;

stats_pack std_dev(int* array, int n){
    double x_sum = 0;
    for (int i = 0; i < n; i++) {
        x_sum += array[i];
    }
    double mean = x_sum/n;
    double square_diff_sum = 0;
    for (int i = 0; i <n ; i++) {
        square_diff_sum += pow(array[i]-mean, 2) ;
    }
    double var = square_diff_sum/n;
    return (stats_pack){mean,sqrt(var)};
}

int main(){
    FILE* inputs;
    int dim[2] = {101,103};
    // int dim[2] = {11,7};
    errno_t err = fopen_s(&inputs,"q14.txt", "r");
    robot_data* all_data = malloc(0);
    int robot_count = 0;
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

        robot_data one_robot; 
        one_robot.p[0] = p_x;
        one_robot.p[1] = p_y;
        one_robot.v[0] = v_x;
        one_robot.v[1] = v_y;


        robot_count++;
        all_data = realloc(all_data, sizeof(robot_data)*robot_count);
        all_data[robot_count-1] = one_robot;
    }
    double best_xstd = 100000;
    double tx = 0;
    double best_ystd = 100000;
    double ty = 0;
    int invW = 0;
    for (int sim_forward = 0;sim_forward < dim[1]; sim_forward++) { //disim_forward[1] = 103
        int* x_cord = malloc(sizeof(int)*robot_count);
        int* y_cord = malloc(sizeof(int)*robot_count);

        //calculating modulo inverse of W
        int test_invW = modulo(sim_forward * dim[0], dim[1]);
        if (test_invW == 1) {
            invW =  sim_forward;
        }

        for (int i = 0; i < robot_count; i++) {
            robot_data one_robot = all_data[i];
            int* p = one_robot.p;
            int* v = one_robot.v;
            position final_pos = simulate_one(dim, p, v,sim_forward);
            x_cord[i] = final_pos.x;
            y_cord[i] = final_pos.y;
        } 
        double xstd = std_dev(x_cord, robot_count).std;
        double ystd = std_dev(y_cord, robot_count).std;
        if (xstd < best_xstd) {
            tx = sim_forward;
            best_xstd = xstd;
        }
        if (ystd < best_ystd) {
            ty = sim_forward; 
            best_ystd = ystd;
        }
        free(x_cord);
        free(y_cord);
    }
    int t = tx + modulo(((ty-tx)*invW), dim[1]) * dim[0];
    printf("%d %f %f\n",t,tx,ty);
    char* map = new_str_map(dim[0], dim[1]);
    for (int i = 0; i < robot_count; i++) {
        robot_data one_robot = all_data[i];
        int* p = one_robot.p;
        int* v = one_robot.v;
        position final_pos = simulate_one(dim, p, v,t);
        int index  =final_pos.y*(dim[0]+1)+final_pos.x;
        map[index] = '*';
    }
    printf("%s\n",map);
    free(map);
    fclose(inputs);
}
