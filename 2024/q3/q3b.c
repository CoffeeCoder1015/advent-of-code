#include <ctype.h>
#include <stdlib.h>
#include <corecrt.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define MUL "mul("
#define DO "do()"
#define DONT "don't()"

int parse_and_calculate(char* mul_call,int n ){
    int X;
    int Y;
    int x_start;
    for (size_t i = 4;i < n;i++) {
        char c = mul_call[i];
        if ( mul_call[i] == '\0' ){
            break;
        }
        if (c == ',') {
            x_start = i+1;  
            int dist = i-4;
            size_t alloc_size = sizeof(char)*(dist+1);
            char* tmp = malloc(alloc_size);
            tmp[0] = 0;
            strncpy_s(tmp,alloc_size , &mul_call[4], dist);
            X = atoi(tmp);
            free(tmp);
        }else if (c == ')'){
            int dist = i-x_start;
            size_t alloc_size = sizeof(char)*(dist+1);
            char* tmp = malloc(alloc_size);
            tmp[0] = 0;
            strncpy_s(tmp,alloc_size , &mul_call[x_start], dist);
            Y = atoi(tmp);
            free(tmp);
        }
    }
    return X*Y;
}
int main(){
    FILE* inputs; 
    errno_t err = fopen_s(&inputs,"q3.txt", "r");

    fseek(inputs, 0, SEEK_END);
    size_t character_count = ftell(inputs);
    rewind(inputs);
    size_t alloc_size = sizeof(char)*(character_count+1);
    char* buffer = malloc(alloc_size);
    fread(buffer,sizeof(char),character_count,inputs);
    fclose(inputs);
    buffer[character_count] = '\0';

    bool captureclose = false;
    bool waiting = false;
    int sum = 0;
    int last = 0;
    for (size_t j = 0; j < character_count-1;j++){
        if(waiting){
            //look for do()
            char check[5];
            size_t copy_amount = 4;
            size_t alloc_size = (copy_amount+1)*sizeof(char);
            strncpy_s(check,alloc_size , &buffer[j],copy_amount);

            if(strcmp(check, DO)==0){
                waiting = false;
                j+=4;
            }else{
                continue;
            }
        }else{
            char check[8];
            size_t copy_amount = 7;
            size_t alloc_size = (copy_amount+1)*sizeof(char);
            strncpy_s(check,alloc_size , &buffer[j],copy_amount);

            if(strcmp(check, DONT)==0){
                waiting = true;
                j+=7;
            }
            //look for don't()
        }
        if(captureclose == false){
            char check[5];
            size_t copy_amount = 4;
            size_t alloc_size = (copy_amount+1)*sizeof(char);
            strncpy_s(check,alloc_size , &buffer[j],copy_amount);

            if(strcmp(check, MUL)==0){
                // printf("%s\n",check);
                captureclose = true;
                last = j;
                j+=4;
            }
        }else{
            char character = buffer[j];
            if(character == ')'){
                int dist = j-last+1;
                size_t alloc_size = (dist+1)*sizeof(char);
                char* str = malloc(alloc_size);
                str[0] = 0;
                errno_t err = strncpy_s(str,alloc_size,&buffer[last],dist);
                sum += parse_and_calculate(str,dist);
                captureclose = false;
                free(str);
            }
            if (isdigit(character)==0 && character != ','){
                captureclose = false;
                j--;
            }
        }

    }
    free(buffer);
    printf("%d",sum);
}
