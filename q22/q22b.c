#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t gen_sec_num(size_t last_sec_num){
    // step 1
    // lsec * 64 => XOR lsec =>  % 16777216
    size_t step1 = last_sec_num << 6;
    last_sec_num ^= step1; 
    last_sec_num %= 16777216;
    // step 2
    // last_sec_num / 32 => mix => prune
    size_t step2 = last_sec_num >> 5;
    last_sec_num ^= step2;
    last_sec_num %= 16777216;
    // step 3
    // last_sec_num * 1024 => mix => prune
    size_t step3 = last_sec_num << 11;
    last_sec_num ^= step3;
    last_sec_num %= 16777216;
    return last_sec_num;
}

int main(){
    FILE* input;
    fopen_s(&input, "test.txt", "rb");

    fseek(input, 0, SEEK_END);
    size_t size = ftell(input);
    rewind(input);
    char* buffer = malloc(size);
    fread(buffer,sizeof(char),size,input);
    fclose(input);
    buffer[size] = '\0';

    char* l_end = strchr(buffer, '\n');
    int line_count = 0;
    while (l_end != NULL) {
        line_count ++; 
        l_end = strchr(l_end+1, '\n');
    }

    int loaded = 0;
    int* store_array = malloc(sizeof(int8_t)*2000*line_count);

    l_end = strchr(buffer, '\n');
    char* l_start = buffer;
    while (l_end != NULL) {
        *l_end = '\0';
        size_t sec = atoll(l_start);
        for (int i = 0; i < 2000; i++) {
            int8_t price = sec%10;
            store_array[i+loaded] = price;
            sec = gen_sec_num(sec);
        }
        loaded++;

        l_start = l_end+1;
        l_end = strchr(l_end+1, '\n');
    }
    free(buffer);

    free(store_array);
}
