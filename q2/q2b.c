#include <corecrt.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

char *intArray_str(int *array, size_t n) {
    size_t doublechars = 0;
    for (size_t i = 0; i < n; i++) {
        if(array[i]>9 || array[i] < 0){
            doublechars++;
        }
    }
    size_t std_alloc_size = sizeof(char) * 3;
    if(doublechars > 0){
        std_alloc_size = sizeof(char) * 3;
    }
    size_t alloc_size = sizeof(char) * ((n-doublechars) * 2 + doublechars*3 + 1);
    char *content = (char *)malloc(alloc_size);
    content[0] = 0;
    for (size_t i = 0; i < n - 1; i++)
    {
        char* buffer = malloc(std_alloc_size);
        sprintf(buffer, "%d,", array[i]);
        strncat_s(content, alloc_size, buffer, std_alloc_size);
        free(buffer);
    }
    char* buffer = malloc(std_alloc_size-1);
    sprintf(buffer, "%d", array[n - 1]);
    strncat_s(content, alloc_size, buffer, std_alloc_size-1);
    free(buffer);
    return content;
}

bool check(int array[], size_t n){
    int last_num = -1;
    int trend = -1;
    for (size_t i = 0; i < n; i++) {
        int number = array[i];
        if(last_num == -1){
            last_num = number;
        }else{
            int diff = number - last_num;

            if (trend == -1){
                trend = number > last_num;
            }
            bool check_c = trend != (number > last_num);
            bool check_s = abs(diff) > 3;
            bool check_0 = diff == 0;
            if (  check_c || check_s ||  check_0){
                return false;
            }
            trend = number > last_num;
        }

        last_num = number;
    }
    return true;
}

int main(){
    FILE* inputs; 
    errno_t err = fopen_s(&inputs,"q2.txt", "r");
    printf("read: %d\n",err);//print err

    size_t faults = 0;
    size_t safes = 0;
    for (size_t i = 0; i < 1000; i++) {
        char buffer[1024];
        char* r = fgets(buffer,1024,inputs);

        int last_num = -1;
        int trend = -1;
        int record[8] = {0,0,0,0,0,0,0,0};
        size_t lo_faults =0;

        size_t last = 0;
        size_t n = 0;
        for (size_t j = 0; j < 1024; j++) {
            char character = buffer[j];
            if(character==' ' || character == '\0' ){
                int dist = j-last;
                size_t alloc_size = (dist+1)*sizeof(char);
                char* str = malloc(alloc_size);
                str[0] = 0;
                errno_t err = strncpy_s(str,alloc_size,&buffer[last],dist);

                int number = atoi(str);
                record[n] = number;

                if(last_num == -1){
                    last_num = number;
                }else{
                    int diff = number - last_num;

                    if (trend == -1){
                        trend = number > last_num;
                    }

                    bool check_c = trend != (number > last_num);
                    bool check_s = abs(diff) > 3;
                    bool check_0 = diff == 0;
                    if (  check_c || check_s ||  check_0){
                        lo_faults++;
                   }
                    trend = number > last_num;
                }

                n++;
                last = j+1;
                last_num = number;
                free(str);
            }
            if(character == '\0'){
                break;
            }
        }
        if (lo_faults > 0){
            faults++;
            for (size_t jk= 0; jk < n; jk++) {
                int size = ( n-1 )*sizeof(int);
                int new[8];
                size_t cpy_size = jk * sizeof(int);
                memcpy(new, record, cpy_size);
                memcpy(&new[jk], &record[jk+1],  size-jk);
                bool res = check(new, n-1);
                if(res){
                    safes++;
                    break;
                }
            }
        }
    }
    printf("unsafe %d",faults-safes);
}
