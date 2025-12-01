#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reset_pin(int (*pin)[5]){
    for (int i = 0; i < 5; i++) {
        (*pin)[i] = 0;
    }
}

void print_pin(int pin[5]){
    for (int i = 0; i < 5; i++) {
        printf("%d ",pin[i]);
    }
    printf("\n");
}

void append_pin(int* pin_count, int (**pins)[5], int pin[5]){
    int size = *pin_count;
    int (*a)[5] = *pins;
    int index = size;
    size++;
    a = realloc(a, sizeof(int[5])*size);
    for (int i = 0; i < 5; i++) {
        a[index][i] = pin[i];
    }
    *pin_count = size;
    *pins = a;
}

bool key_fit(int key[5], int schem[5]){
    for (int i = 0; i < 5; i++) {
        if (key[i] + schem[i] > 5) {
            return false; 
        } 
    }
    return true;
}

int main(){
    FILE* inputs;
    fopen_s(&inputs, "q25.txt", "r");


    int key_count = 0;
    int (*keys)[5] = malloc(0);

    int schem_count = 0;
    int (*schems)[5] = malloc(0);

    char buffer[1024];
    int counter = 0;
    bool isschem = false;
    int pin[5] = {0,0,0,0,0};
    for (;;) {
        char* end = fgets(buffer, 1024, inputs);
        if (end == NULL) {
            break; 
        }
        if (buffer[0] == '\n') {
            counter = 0;
            if (isschem) {
                append_pin(&schem_count, &schems, pin);
            }else {
                append_pin(&key_count, &keys, pin);
            }
            reset_pin(&pin);
            continue; 
        }

        int n = strlen(buffer);
        buffer[n-1] = '\0';

        if (counter == 0) {
            isschem = strcmp(buffer, "#####") == 0;
        }else if (counter < 6) {
            for (int i = 0;  i < 5; i++) {
                char c = buffer[i];
                if (c == '#') {
                    pin[i]++;
                }
            } 
        }
        counter++;
    }
    if (isschem) {
        append_pin(&schem_count, &schems, pin);
    }else {
        append_pin(&key_count, &keys, pin);
    }
    fclose(inputs);

    uint64_t fit = 0;
    for (int i = 0; i < schem_count; i++) {
        int *schem = schems[i];
        for (int j = 0; j < key_count; j++) {
            int *key = keys[j];
            if (key_fit(key, schem)) {
               fit++;  
            }
        } 
    }
    printf("%llu\n",fit);
}
