#include <stddef.h>
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
    buffer[size] = '\0';

    char* l_end = strchr(buffer, '\n');
    int line_count = 0;
    while (l_end != NULL) {
        line_count ++; 
        l_end = strchr(l_end+1, '\n');
    }

    size_t sum = 0;
    for (;;) {
        char buffer[1024];
        char* end = fgets(buffer, 1024 , input);
        if (end == NULL) {
            break; 
        }
        int n = strlen(buffer);
        buffer[--n] = '\0';
        size_t inital = atoll(buffer);
        size_t p = inital;
        for (int i = 0; i < 2000; i++) {
            p = gen_sec_num(p);
        }
        sum+=p;
    }
    printf("%llu\n",sum);
}
