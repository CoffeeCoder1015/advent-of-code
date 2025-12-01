#include <corecrt.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


int main(){
    FILE* inputs; 
    errno_t err = fopen_s(&inputs,"q2.txt", "r");
    printf("read: %d\n",err);//print err
    size_t faults = 0;
    size_t n = 0;
    for (size_t i = 0; i < 1000; i++) {
        char buffer[1024];
        char* r = fgets(buffer,1024,inputs);
        n++;

        int last_num = -1;
        int trend = -1;

        size_t last = 0;
        for (size_t j = 0; j < 1024; j++) {
            char character = buffer[j];
            if(character==' ' || character == '\0' ){
                int dist = j-last;
                size_t alloc_size = (dist+1)*sizeof(char);
                char* str = malloc(alloc_size);
                str[0] = 0;
                errno_t err = strncpy_s(str,alloc_size,&buffer[last],dist);
                // errno_t err = memcpy_s(str, alloc_size+2, &buffer[last], dist);
                int number = atoi(str);
                last = j+1;

                if(last_num == -1){
                    last_num = number;
                    continue;
                }

                if(number==last_num||abs(number-last_num)>3){ //change too big
                    faults++;
                    break;
                }

                if (trend == -1){
                    trend = number > last_num;
                    last_num = number;
                    continue;
                }

                if ((number > last_num) != trend){ // trend change
                    faults++;
                    break;
                }

                last_num = number;
                free(str);
            }
            if(character == '\0'){
                break;
            }
        }
    }
    printf("unsafe %d",faults);
    fclose(inputs);


}
