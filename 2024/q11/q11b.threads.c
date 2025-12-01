/*
 *
 *
 * NOTE: DOESNT ACTUALLY WORK
 *
 *
 */

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

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


typedef struct{
    uint64_t key;
    uint64_t value[3];
} map_entry;

typedef struct{
    map_entry **entries;
    size_t capacity;    // size of _entries array
    size_t length;  // number of items in hash table
} hashmap;

void print_hashmap(hashmap* h) {
    for (size_t i = 0; i < h->capacity; i++) {
        if (h->entries[i]) {
            printf("Index %zu: Key=%llu, Value[0]=%llu, Value[1]=%llu Value[2]=%llu\n",
                   i, h->entries[i]->key, h->entries[i]->value[0], h->entries[i]->value[1],h->entries[i]->value[2]);
        }
    }
}
#define INIT_CAPACITY 3200
hashmap* new_hashmap(){
    hashmap* h = malloc(sizeof(hashmap));
    h->length = 0;
    h->capacity = INIT_CAPACITY;

    h->entries = calloc(h->capacity, sizeof(map_entry*));
    return h;
}

void free_hashmap(hashmap* h){
    //free entries
    // for (int i = 0; i < h->capacity; i++) {
    //     free(*(map_entry**)( h->entries[i] ));
    // }
    for (size_t i = 0; i < h->capacity; i++) {
        if (h->entries[i]) {
            free(h->entries[i]);
        }
    }
    free(h->entries); //frees entries pointer array
    free(h);
}


#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
static uint64_t hash_key(uint64_t key) {
    uint64_t hash = FNV_OFFSET;
    for (int byte_offset= 0; byte_offset < 8; byte_offset++) {
        uint8_t byte_of_data = ( key >> (byte_offset * 8) ) & 255;
        hash *= FNV_PRIME;
        hash ^= byte_of_data;
    }
    return hash;
}

uint64_t* map_set_entry(map_entry** entries, size_t capacity, size_t* plength, uint64_t key, uint64_t value[3]) {
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

    while (entries[index] != NULL) {
        uint64_t current_key = entries[index]->key;
        if (current_key == key) {
            // Found key (it already exists), update value.
            entries[index]->value[0] = value[0];
            entries[index]->value[1] = value[1];
            entries[index]->value[2] = value[2];
            return &entries[index]->key;
        }
        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }
    }
    // Didn't find key, allocate+copy if needed, then insert it.
    if (plength != NULL) {
        (*plength)++;
    }
    entries[index] = (map_entry*)malloc(sizeof(map_entry));
    entries[index]->key = key;
    entries[index]->value[0] = value[0];
    entries[index]->value[1] = value[1];
    entries[index]->value[2] = value[2];
    return &entries[index]->key;
}
bool map_expand(hashmap* h){
    // Allocate new entries array.
    size_t new_capacity = h->capacity * 2;
    printf("TOO SMALL! %d\n",new_capacity);
    if (new_capacity < h->capacity) {
        return false;  // overflow (capacity would be too big)
    }
    map_entry** new_entries = calloc(new_capacity, sizeof(map_entry*));
    if (new_entries == NULL) {
        return false;
    }

    // Iterate entries, move all non-empty ones to new h's entries.
    for (size_t i = 0; i < h->capacity; i++) {
        map_entry* entry = h->entries[i];
        if (entry != NULL) {
            map_set_entry(new_entries,new_capacity,NULL,entry->key,entry->value);
            free(entry);
        }
    }

    // Free old entries array and update this h's details.
    free(h->entries);
    map_entry*** reassign_entry = &h->entries;
    *reassign_entry = new_entries;
    h->capacity = new_capacity;
    return true;
}

uint64_t* map_get(hashmap* h, uint64_t key){
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(h->capacity - 1));
    if (h->length == 0) {
        return NULL; 
    }
    while (h->entries[index] != NULL) {
        uint64_t current_key = h->entries[index]->key;
        if (current_key == key) {
            return h->entries[index]->value ;
        } 
        index++;
        if (index >= h->capacity) {
            index = 0; 
        }
    }
    return NULL;
}

void* map_set(hashmap* h, uint64_t key, uint64_t value[3]){
    if (h->length >= h->capacity / 2) {
        if (!map_expand(h)) {
            return NULL;
        }
        printf("Expanded!\n");
    }

    void* result = map_set_entry(h->entries,h->capacity,&h->length,key,value);
    return result;

}

typedef void* any_ptr;
typedef struct{
   any_ptr real_ptr; //final pointer
} shared_ptr;

typedef struct {
    unsigned long long current;
    // int* next_buffer_count;
    // shared_ptr* next_buffer;
    int local_count;
    unsigned long long* local_buffer;
} thread_args;

void realloc_real(shared_ptr* sptr,size_t _NewSize){
    //any_ptr* is used to the reassignment is modifies the real pointer.
    void* tmp =realloc(sptr->real_ptr, _NewSize);
    if (tmp == NULL) {
        printf("REALLOC FAILED\n");
        exit(1);
    }
    sptr->real_ptr = tmp;
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void* processNum(void* args){
    hashmap* calculations = new_hashmap(); 
    // unsigned long long current = working_buffer[j];
    thread_args* a = (thread_args*)args;
    unsigned long long gcurrent = a->current;
    // printf("thread doing %d\n",current);

    int lworking_c = 1;
    unsigned long long* lworking  = malloc(sizeof(unsigned long long));
    lworking[0] = gcurrent;


    int lnext_c = 0;
    unsigned long long *lnext = malloc(0);
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < lworking_c; j++) {
            unsigned long long last = 0;
            unsigned long long last_2nd = 0;
            bool split = false;
            unsigned long long current = lworking[j];

            // pthread_mutex_lock(&mutex);
            uint64_t* value = map_get(calculations, current);
            // pthread_mutex_unlock(&mutex);

            if (value == NULL) {
                if(current == 0){
                    last = 1;
                }else{
                    double digit_count = floor(log10(current))+1;
                    split = (int)digit_count % 2 == 0;
                    if (split) {
                        double half_dl = digit_count/2;
                        double shift = pow(10, half_dl);
                        last_2nd = current/shift;
                        last = current-last_2nd*shift;
                    }else {
                        last = current*2024;
                    }
                }
                uint64_t set_value[3] = {0,last,last_2nd};
                if (split) {
                    set_value[0] = 1;
                }
                // pthread_mutex_lock(&mutex);
                void* result = map_set(calculations,current,set_value);
                if (result == NULL) {
                    printf("SET FAILED");
                }
                // pthread_mutex_unlock(&mutex);
            }else {
                last = value[1];
                split = value[0] == 1;
                last_2nd = value[2];
            }
            lnext_c+=1+split;
            lnext = realloc(lnext,sizeof(unsigned long long)*lnext_c);
            lnext[lnext_c-1] = last;
            if (split) {
                lnext[lnext_c-2] = last_2nd;
            }

        }
        unsigned long long* old = lworking;
        lworking_c = lnext_c;
        lworking = lnext;
        lnext_c = 0;
        lnext = old;
    }
    int *lc = &( a->local_count );
    *lc = lworking_c;
    a->local_buffer= lworking;
    // print_hashmap(calculations);
    free_hashmap(calculations);
    // pthread_mutex_lock(&mutex);
    // printf("## - [T %lld] - ##\n",gcurrent);
    // for (int i = 0;i<lworking_c; i++) {
    //     printf("%lld ",lworking[i]) ;
    // }
    // printf("\n");
    // printf("#############\n");
    // pthread_mutex_unlock(&mutex);
    // for (int i = 0;i<*lc; i++) {
    //     printf("%d ",a->local_buffer[i]) ;
    // }
    // printf("\n");
    
    // pthread_mutex_lock(a->mutex);
    // int* next_buffer_count = a->next_buffer_count;
    // shared_ptr* next_buffer = a->next_buffer;
    // ( *next_buffer_count )+=1+split;
    //
    // size_t alloc_size = sizeof(unsigned long long)*( *next_buffer_count );
    // realloc_real(next_buffer, alloc_size);
    //
    // unsigned long long** real_loc = (long long**)&( next_buffer->real_ptr );
    // (*real_loc)[*next_buffer_count-1] = last;
    // if (split) {
    //     (*real_loc)[*next_buffer_count-2] = last_2nd;
    // }
    // // memcpy_s(&(*real_loc)[original_count], alloc_size, lworking_buffer, alloc_size);
    // // for (int i = 0; i < *next_buffer_count; i ++) {
    // //     printf("%d ",( *real_loc )[i]);
    // // }
    // // printf("\n");
    // pthread_mutex_unlock(a->mutex);
    return 0;
}


int main(){
    FILE* inputs; 
    errno_t err = fopen_s(&inputs,"q11.txt", "r");
    char buffer[1024];
    fgets(buffer, 1024, inputs);
    fclose(inputs);
    char* next_line = strchr(buffer, '\n');
    *next_line = '\0';

    int input_count = 0;
    char** split_input = split(buffer, ' ');
    for (int i = 0; split_input[i] != 0; i++) {
        input_count = i+1; 
    }
    int working_buffer_count = input_count;
    unsigned long long* working_buffer = malloc(sizeof(long long)*working_buffer_count);
    for (int i = 0; i < working_buffer_count; i++) {
        unsigned long long to_num = strtoll(split_input[i], NULL, 10);
        working_buffer[i] = to_num;
    }
    //buffer swap:
    //working buffer
    //make next buffer
    //
    //free working buffer
    //working buffer pointer = next buffer
    //make next buffer
    

    // int next_buffer_count = 0;
    // unsigned long long* next_buffer = malloc(0);
    //
    // //create shared ptr
    // shared_ptr nb_sptr ;
    // nb_sptr.real_ptr = next_buffer; 

    #define THREAD_COUNT 1000
    int thread_count = THREAD_COUNT;
    pthread_t threads_list[THREAD_COUNT];

    int args_count = 0;
    for (int i = 0; i < 1; i++) {
        thread_args* arguments = malloc(sizeof(thread_args)*working_buffer_count);
        int true_thread_count = 0;
        for (int j = 0; j < working_buffer_count; j++) {
            arguments[j].current = working_buffer[j];
            // arguments[j].next_buffer_count = &next_buffer_count;
            // arguments[j].next_buffer = &nb_sptr;

            true_thread_count++;
            // pthread_attr_t attr;
            // pthread_attr_init(&attr);
            // pthread_attr_setstacksize(&attr, (long long)2048*2048*2048*2048);  // Set 1MB stack size
            
            pthread_create(&threads_list[j%thread_count], NULL, processNum, &arguments[j]);

            // pthread_attr_destroy(&attr);

            if ((j+1)%thread_count == 0) {
                // printf("%d\n",j);
                for (int k = 0; k < thread_count; k++) {
                    pthread_join(threads_list[k%thread_count], NULL) ;
                }
                true_thread_count = 0;
            }
        }
        for (int j = 0; j<true_thread_count; j++) {
            pthread_join(threads_list[j], NULL) ;
        }

        int tc = 0;
        for (int j = 0; j < working_buffer_count; j++) {
            tc += arguments[j].local_count;
        }

        working_buffer = realloc(working_buffer, sizeof(unsigned long long)*tc);
        int index = 0;
        for (int j = 0; j < working_buffer_count; j++) {
            // memcpy_s(&working_buffer[index], sizeof(unsigned long long)*tc, arguments[j].local_buffer, arguments[j].local_count*sizeof(long long));
            for (int k = index; k < index+arguments[j].local_count; k++) {
                working_buffer[k] = arguments[j].local_buffer[k-index];
            }
            index+=arguments[j].local_count;
        }
        working_buffer_count = tc;
        // printf("%d\n",working_buffer_count);

        // unsigned long long* old_wb_address = working_buffer;
        // working_buffer = (unsigned long long*)nb_sptr.real_ptr;
        // working_buffer_count = next_buffer_count;

        // next_buffer_count = 0;
        // next_buffer = old_wb_address;
        // nb_sptr.real_ptr = next_buffer;
        args_count = 0;
        free(arguments);
    }
    // for (int i = 0; i<working_buffer_count; i++) {
    //     printf("%d ",working_buffer[i]) ;
    // }
    // print_hashmap(calculations);
    printf("\r\n%d\n",working_buffer_count);
    free(working_buffer);
    // free(hash);
    // free(next_buffer);
}
