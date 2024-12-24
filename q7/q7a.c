#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
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


char** split(char string[], char delim){
    size_t delim_count = 0;
    char* last_delim = 0;
    char *pch=strchr(string,delim);
    while (pch!=NULL) {
        delim_count++;
        last_delim = pch;
        pch=strchr(pch+1,delim);
    }

    /* add null byte */
    delim_count+=2;

    char** strarray = malloc(sizeof(char*)*delim_count);

    char* next_tok = NULL;
    char delim_str[2] = {delim, '\0'};
    char* token = strtok_s(string, delim_str, &next_tok);
    int i = 0;
    while (token != NULL) {
        char* s  =  _strdup(token);
        strarray[i] = s;
        // printf("%d %s\n",i,s);
        token = strtok_s(NULL,delim_str,&next_tok);
        i++;
    }
    strarray[i] = 0;
    return  strarray;
}

bool process(long long ex, long long* seq, int seq_n){
    int raw_init = 0;

    int weight_count = seq_n-1;
    double MAX_ITERS = pow(2,weight_count);
    int max = (1<<weight_count)-1;
    int order_id = raw_init&max;

    bool R = false;
    for (int search=0; search < MAX_ITERS; search++) {
        long long sum = seq[0];
        for (int i = 0; i < weight_count; i++) {
            int last = ( order_id>>i )&1;
            switch (last) {
                case 0: 
                    sum += seq[i+1];
                    break;
                case 1: 
                    sum *= seq[i+1];
                    break;
            }
        }

        if (sum == ex) {
            R = true;
            break;
        }

        order_id++;
        if (order_id == max+1) {
            break;
        }
    }
    return R;
}

int main(){
    FILE* inputs;
    errno_t err = fopen_s(&inputs,"q7.txt", "r");

    long long sum = 0;
    for(;;){
        char buffer[1024];
        char* r = fgets(buffer, 1024, inputs);
        if (r == NULL) {
            break; 
        }
        buffer[strcspn(buffer, "\n")] = 0;
        char* target_divider = strchr(buffer, ':');
        *target_divider = '\0';
        char* sequence = target_divider+1;

        char** seq_split = split(sequence, ' ');
        long long Expected = atoll(buffer);

        int amount_of_data = 0;
        long long* Data = malloc(0);
        size_t i;
        for (i = 0; seq_split[i] != 0; i++) {
            amount_of_data++;
            Data = realloc(Data, sizeof(long long)*amount_of_data);
            Data[amount_of_data-1] = atoll(seq_split[i]);
            free(seq_split[i]);
        }
        free(seq_split[i]);
        free(seq_split);

        if(process(Expected, Data, amount_of_data)){
            sum += Expected;
        }

        free(Data);
    }

    fclose(inputs);
    printf("%lld\n",sum);
}
