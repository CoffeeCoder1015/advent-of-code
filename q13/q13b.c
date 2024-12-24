#include <math.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_float_array(double *arr, int n){
    for (int i = 0 ; i < n; i++) {
        printf("%.2f ",arr[i]);
    }
    printf("\n");
}
void print_matrix(double** m, int rows, int cols){
    printf("--------------------------\n");
    for (int y =0 ; y < rows; y++) {
        print_float_array(m[y], cols);
    }
    printf("--------------------------\n");
}

// major: matrix can be row major or column by major
// M [row] [column] or M [column] [row]
double** new_matrix(int rows, int columns,int major){
    int dimensons[2] = {rows,columns};
    major %=2;
    double** M = calloc(dimensons[major],sizeof(double*));
    for (int i = 0; i < dimensons[major]; i++) {
        M[i] = calloc(dimensons[(major+1)%2], sizeof(double));
    }
    return M;
}

// major: matrix can be row major or column by major
// M [row] [column] or M [column] [row]
void free_matrix(double** M,int rows, int columns,int major){

    int dimensons[2] = {rows,columns};
    major %=2;
    for (int i = 0; i < dimensons[major]; i++) {
        free(M[i]);
    }
    free(M);
}

void matrix_swap_rows(double** M, int rowA, int rowB,int rows){
    bool rowA_in = 0<=rowA && rowA < rows;
    bool rowB_in = 0<=rowB && rowB < rows;
    if (!( rowA_in && rowB_in )) {
        return; 
    }
    double* ptrA = M[rowA];
    M[rowA] = M[rowB];
    M[rowB] = ptrA;
}

// copies selected row, multiplies by factor and returns it;
double* matrix_row_mult(double** M, int rows,int columns, double m_factor,int row){
    if (!( 0 <= row && row < rows )) {
        return NULL; 
    }
    size_t alloc_size = sizeof(double)*columns;
    double* dup_row = malloc(alloc_size);
    memcpy_s(dup_row, alloc_size, M[row], alloc_size);
    for (int i = 0; i < columns; i++) {
        dup_row[i] *= m_factor;
    }
    return dup_row;
}

// multiplies selected row directly
void matrix_row_mult_inplace(double** M, int rows, int columns, double m_factor, int row){
    if (!( 0 <= row && row < rows )) {
        return; 
    }
    for (int i = 0; i < columns; i++) {
        M[row][i] *= m_factor;
    }
}

// array must be same len as columns
void matrix_row_add_array(double** M,int rows, int columns,double* array, int row){
    if (!( 0 <= row && row < rows )) {
        return; 
    }
    for (int i = 0; i < columns; i++) {
        M[row][i] += array[i];
    }
}

void matrix_row_add_other_with_factor(double** M,int rows, int columns,double m_factor,int rowDest, int rowMult){
    bool rowDest_in = 0<=rowDest && rowDest < rows;
    bool rowMult_in = 0<=rowMult && rowMult < rows;
    if (!( rowDest_in && rowMult_in )) {
        return; 
    }
    double* multiplied = matrix_row_mult(M,  rows,  columns,  m_factor,  rowMult);
    for (int i = 0; i < columns; i++) {
        // printf("got here %f\n",multiplied[i]);
        M[rowDest][i] += multiplied[i];
    }
    free(multiplied);
}

int matrix_column_max_index(double** M, int rows, int columns, int row, int column){
    bool row_in = 0<=row && row < rows;
    bool column_in = 0<=column && column < columns;
    if (!( row_in && column_in )) {
        return -1; 
    }
    double max = M[row][column];
    int max_index = row;
    for (int i = row+1; i < rows; i++) {
        double current = M[i][column];
        if (current > max) {
            max = current; 
            max_index = i;
        }
    }
    return max_index;
}

void matrix_Gaussian_Elim(double** M, int rows, int columns){
    int ccolumn = 0;
    for (int crow = 0; crow < rows-1; crow++) {
        int max_row = matrix_column_max_index(M, rows, columns, crow, ccolumn);
        if (max_row != crow) {
            matrix_swap_rows(M, max_row, crow, rows);
        }
        for (int reduce_row = crow+1; reduce_row < rows; reduce_row++) {
            double m_factor = M[reduce_row][ccolumn]/M[crow][ccolumn];
            // printf("%f\n",m_factor);
            matrix_row_add_other_with_factor(M, rows, columns, -m_factor, reduce_row, crow);
        }
    }
}

void matrix_GaussianJorde_Elim(double** M, int rows, int columns){
    int ccolumn = 0;
    for (int crow = 0; crow < rows; crow++) {
        int max_row = matrix_column_max_index(M, rows, columns, crow, ccolumn);
        if (max_row != crow) {
            matrix_swap_rows(M, max_row, crow, rows);
        }
        double to_1_factor = 1/(M[crow][ccolumn]);
        matrix_row_mult_inplace(M, rows, columns, to_1_factor, crow);
        for (int reduce_row = crow+1; reduce_row < rows; reduce_row++) {
            double m_factor = M[reduce_row][ccolumn]/M[crow][ccolumn];
            // printf("%f\n",m_factor);
            matrix_row_add_other_with_factor(M, rows, columns, -m_factor, reduce_row, crow);
        }
        for (int back_reduce_row = crow-1; 0<=back_reduce_row ; back_reduce_row--) {
            double m_factor = M[back_reduce_row][ccolumn]/M[crow][ccolumn];
            // printf("%f\n",m_factor);
            matrix_row_add_other_with_factor(M, rows, columns, -m_factor, back_reduce_row, crow);
        }
        ccolumn++;
    }
}

bool isInt(double number){
    double max = ceil(number);
    double max_diff = fabs(max-number);
    bool max_highpres = max_diff < 0.001;
    double min = floor(number);
    double min_diff = fabs(min-number);
    bool min_highpres = min_diff < 0.001;
    return max_highpres | min_highpres;
}

int main(){
    FILE* inputs;
    errno_t err = fopen_s(&inputs,"q13.txt", "r");
    int line_count = 0;
    bool reached_end = false;
    unsigned long long tokens = 0;
    int processed = 0;
    for (;!reached_end;) {
        double** matrix = new_matrix(2, 3, 0);
        for (int i = 0; i < 4; i++) {
            char buffer[1024];
            char* r = fgets(buffer, 1024, inputs);
            if (r == NULL) {
                reached_end = true;
                break; 
            }
            if (buffer[0] == '\n') {
                line_count = 0;
                continue; 
            }
            if (line_count == 1 || line_count == 0) {
                char* x_start = strchr(buffer, '+')+1;
                char* x_end = strchr(x_start, ',');

                char* y_start = strchr(x_start, '+')+1;
                char* y_end = strchr(y_start, '\n');
                *x_end = '\0';
                *y_end = '\0';
                double x = atof(x_start);
                double y = atof(y_start);

                // printf("%d %d\n",x,y);
                matrix[0][line_count] = x;
                matrix[1][line_count] = y;
            }else{
                char* x_start = strchr(buffer, '=')+1;
                char* x_end = strchr(x_start, ',');

                char* y_start = strchr(x_start, '=')+1;
                char* y_end = strchr(y_start, '\n');
                *x_end = '\0';
                *y_end = '\0';
                double x = atof(x_start);
                double y = atof(y_start);
                matrix[0][2] = x+10000000000000;
                matrix[1][2] = y+10000000000000;
                matrix_GaussianJorde_Elim(matrix, 2, 3);
                double required_A = matrix[0][2];
                double required_B = matrix[1][2];
                bool valid_a = isInt(required_A)  && required_A > 100;
                bool valid_b = isInt(required_B) && required_B > 100;
                if (valid_a && valid_b) {
                    tokens += round(required_A) * 3 + round(required_B);
                }
                processed++;
            }
            line_count++;
            line_count%=3;
        }
        free_matrix(matrix, 2, 3, 0);
    }
    fclose(inputs);
    printf("%llu\n",tokens);
}
