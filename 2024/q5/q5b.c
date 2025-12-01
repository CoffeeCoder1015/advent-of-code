#include <stdlib.h>
#include <corecrt.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>


int compare_func_desc(const void *a, const void *b)
{
    int i_a = *((int *)a);
    int i_b = *((int *)b);

    if (i_a > i_b)
        return -1;
    else if (i_a == i_b)
        return 0;
    else
        return 1;
}
int compare_func(const void *a, const void *b)
{
    int i_a = *((int *)a);
    int i_b = *((int *)b);

    if (i_a < i_b)
        return -1;
    else if (i_a == i_b)
        return 0;
    else
        return 1;
}

int* get_keys(int** map,size_t n){
    size_t key_count = 0;
    int* keys = malloc(0);
    for (size_t i = 0; i < n; i++) {
        int* current_key =  map[i];
        if(current_key != 0){
            //append to keys
            key_count++;
            int* keys_tmp = realloc(keys, sizeof(int)*( key_count+1) );
            if(keys_tmp == NULL){
                printf("get keys failed to alloc new space\n");
                exit(1);
            }
            keys = keys_tmp;
            keys[key_count-1] = i;
        }
    }
    keys[key_count] = 0;
    return keys;
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

bool inArray(int check, int* array, int n){
    bool found = false;
    for(size_t i = 0; i<n ;i++){
        int current = array[i];
        // printf("%d == %d\n",check,curent);
        if(check == current){
            found = true;
            break;
        }
    }
    return found;
}

int mid(int* array, int n ){
    bool true_mid = n%2;
    if(!true_mid){
        return -1;
    }
    int trunc_div = ( n-1 )/2;
    return  array[trunc_div];
}

int unmap_n(int mapn_n, int* map, int n){
    for (size_t i = 0; i< n; i++) {
        int current = map[i];
        if(mapn_n == current){
            return i;
        }
    }
    return -1;
}

int main(){
    FILE* inputs; 
    errno_t err = fopen_s(&inputs,"q5.txt", "r");

    //*    Load rules into a simple map    *//
    int** map = calloc(100, sizeof(int*)); //init empty map
    size_t key_max = 0;
    for (;;) {
        char buffer[1024];
        char* r = fgets(buffer, 1024, inputs);
        buffer[strcspn(buffer, "\n")] = 0;
        if(buffer[0] == '\0'){
            break;
        }
        buffer[2] = '\0';
        int n1 = atoi(buffer);
        int n2 = atoi(&buffer[3]);
        // if(n1 > key_max){
        //     key_max = n1;
        //     size_t alloc_size =sizeof(int)*( key_max + 2 );
        //     int** map_tmp = realloc(map,alloc_size);
        //     if(map_tmp == NULL){
        //         printf("map size increase failed. attemped %d\n",alloc_size);
        //         return 1;
        //     }
        //     map = map_tmp;
        //     
        // }

        //if first key
        if (map[n1] == NULL){
            int* iarray = malloc(sizeof(int)*2);
            iarray[0] = 1;
            iarray[1] = n2;
            map[n1] = iarray;
        }else{
            int* iarray = map[n1];
            int* iarray_tmp = realloc(iarray, sizeof(int)*(iarray[0]+2));
            if(iarray == NULL){
                printf("failed to increase value array size key:%d\n",n1);
                return 1;
            }
            map[n1] = iarray_tmp;
            iarray = map[n1];
            iarray[0]++;
            iarray[iarray[0]]=n2;

        }
    }

    //* Get keys in the map *//
    int* keys = get_keys(map, 100);
    size_t keys_n = 0;
    for (int i = 0; keys[i] != 0; i++) {
        keys_n = i+1;
        qsort(&map[keys[i]][1],map[keys[i]][0],sizeof(int),compare_func);
    }

    //* Process each update *//
    int ans_sum = 0;
    for (;;) {
        char buffer[1024];
        char* r = fgets(buffer, 1024, inputs);
        if(r == NULL){
            printf("File End\n");
            break;
        }

        //*   Chop off \n   *//
        printf("update: %s",buffer);
        for (size_t i = 0; i < 1024; i++) {
            char c = buffer[i];
            if(c == '\n'){
                buffer[i] = '\0';
                break;
            }
        }

        //* Convert updates from string array to int array *//
        char** stringarr = split(buffer,',');
        int n=0;
        int* updates_sequence = malloc(0);
        for (size_t i = 0; stringarr[i] != 0; i++) {
            n = i+1;
            updates_sequence = realloc(updates_sequence, sizeof(int)*n);
            updates_sequence[i] = atoi(stringarr[i]);
            free(stringarr[i]);
        }
        free(stringarr);

        //*  Weight each page update *//
        int* weighted_updates = malloc(sizeof(int)*n); //kv match to update_sequence
        bool broken = false;
        for (size_t i =0; i<n; i++) {
            int page = updates_sequence[i];
            int match_count = 0; //this is the weight
            if(inArray(page, keys, keys_n)){
                int search_n = map[page][0]; //all rules for a page
                int* search_space = &map[page][1];
                for (size_t j = 0; j < n; j++) { //count the pages in the rule which appear in this update
                    match_count += inArray(updates_sequence[j], search_space, search_n);
                }

                //check if the update (as a whole, i.e task from q5a) is actually broken
                int next_page = updates_sequence[i+1];
                bool found = inArray(next_page, search_space, search_n);
                if(found == false && i+1 != n){
                    broken = true;
                }
            }else if (i+1 != n){ //check if the page is a end-node; the last page wont have a key entry
                broken = true;
            }
            weighted_updates[i] = match_count;
        }


        //* Make a sort array so the old one can be used as a key-map *//
        int* sorted_updates = malloc(sizeof(int)*n);
        memcpy(sorted_updates, weighted_updates, sizeof(int)*n);
        qsort(sorted_updates, n, sizeof(int), compare_func_desc);

        //* If update is broken, use mid point fromm fixed update *//
        int m = mid(sorted_updates,n);
        if (broken){
            if(m == -1){ //just a check to see if a true midpoint can be actually found
                printf("No true mid\n");
            }else{
                //* Remap the page value to the weight value and add to the answer sum*//
                int lookup_index = unmap_n(m, weighted_updates, n);
                int unmapped = updates_sequence[lookup_index];
                if(unmapped == -1){
                    printf("cannot unmap %d\n",m);
                }
                ans_sum += unmapped;
            }
        }
        printf("\n");
        free(updates_sequence);
        free(weighted_updates);
        free(sorted_updates);
    }
    printf("%d\n",ans_sum);

    for (int i = 0; i < keys_n; i++) {
        free(map[keys[i]]);
    }
    free(keys);
    free(map);
    ///*   Read Entire File    *///
    // fseek(inputs, 0, SEEK_END);
    // size_t character_count = ftell(inputs);
    // rewind(inputs);
    //
    // size_t alloc_size = sizeof(char)*(character_count+1);
    // char* buffer = malloc(alloc_size);
    // fread(buffer,sizeof(char),character_count,inputs);
    //
    // fclose(inputs);
    // buffer[character_count] = '\0';
}
