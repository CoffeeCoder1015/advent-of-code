#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
        token = strtok_s(NULL,delim_str,&next_tok);
        i++;
    }
    strarray[i] = 0;
    return  strarray;
}


// 
// NOTE: Hashap is a modified version from https://benhoyt.com/writings/hash-table-in-c/
//
typedef struct{
    uint64_t key;
    uint64_t value;
} map_entry;

typedef struct{
    map_entry **entries;
    size_t capacity;    // size of _entries array
    size_t length;  // number of items in hash table
} hashmap;

void print_hashmap(hashmap* h) {
    for (size_t i = 0; i < h->capacity; i++) {
        if (h->entries[i]) {
            printf("Index %zu: Key=%llu, Value[0]=%llu\n",
                   i, h->entries[i]->key, h->entries[i]->value);
        }
    }
}
#define INIT_CAPACITY 6400
hashmap* new_hashmap(){
    hashmap* h = malloc(sizeof(hashmap));
    h->length = 0;
    h->capacity = INIT_CAPACITY;

    h->entries = calloc(h->capacity, sizeof(map_entry*));
    return h;
}

void free_hashmap(hashmap* h){
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

uint64_t* map_set_entry(map_entry** entries, size_t capacity, size_t* plength, uint64_t key, uint64_t value) {
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

    while (entries[index] != NULL) {
        uint64_t current_key = entries[index]->key;
        if (current_key == key) {
            // Found key (it already exists), update value.
            entries[index]->value = value;
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
    entries[index]->value = value;
    return &entries[index]->key;
}
bool map_expand(hashmap* h){
    // Allocate new entries array.
    size_t new_capacity = h->capacity * 2;
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
            return &h->entries[index]->value ;
        } 
        index++;
        if (index >= h->capacity) {
            index = 0; 
        }
    }
    return NULL;
}

void* map_set(hashmap* h, uint64_t key, uint64_t value){
    if (h->length >= h->capacity / 2) {
        if (!map_expand(h)) {
            return NULL;
        }
    }

    void* result = map_set_entry(h->entries,h->capacity,&h->length,key,value);
    return result;

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

    int keycount = input_count;
    uint64_t* keys = malloc(sizeof(uint64_t)*keycount);

    hashmap* stones = new_hashmap();
    for (int i = 0; i < input_count; i++) {
        unsigned long long to_num = strtoll(split_input[i], NULL, 10);
        map_set(stones, to_num, 1);
        keys[i] = to_num;
    }

    for (int i = 0; i < 75; i++) {
        hashmap* new_stones = new_hashmap();
        int nkeycount = 0;
        uint64_t* nkeys = malloc(sizeof(uint64_t)*nkeycount);
        for (int j = 0; j < keycount; j++) {
            uint64_t current_count = *map_get(stones, keys[j]);
            uint64_t current = keys[j];
            unsigned long long last = 0;
            unsigned long long last_2nd = 0;
            bool split = false;
            if(current == 0){
                last = 1;
            }else{
                long long l10 = log10(current);
                long long digit_count = l10+1;
                split = (int)digit_count % 2 == 0;
                if (split) {
                    long long half_dl = digit_count/2;
                    long long shift = pow(10, half_dl);
                    last_2nd = current/shift;
                    last = current-last_2nd*shift;
                }else {
                    last = current*2024;
                }
            }
            uint64_t* new_stone_current = map_get(new_stones, last);
            uint64_t nstone_val = 0;
            if (new_stone_current != NULL) { // key exist
                nstone_val = *new_stone_current; 
            }else{
                nkeycount++;
                nkeys = realloc(nkeys, sizeof(uint64_t)*nkeycount);
                nkeys[nkeycount-1] = last;
            }
            map_set(new_stones, last, current_count+nstone_val);
            if (split) {
                uint64_t* new_stone_current2 = map_get(new_stones, last_2nd);
                uint64_t nstone_val2 = 0;
                if (new_stone_current2 != NULL) { //key exist
                    nstone_val2 = *new_stone_current2; 
                }else {
                    nkeycount++;
                    nkeys = realloc(nkeys, sizeof(uint64_t)*nkeycount);
                    nkeys[nkeycount-1]  = last_2nd;
                }
                map_set(new_stones, last_2nd, current_count+nstone_val2);
            }
        }
        free(keys);
        keycount = nkeycount;
        keys = nkeys;

        free_hashmap(stones);
        stones = new_stones;
    }
    uint64_t sum = 0;
    for (int i =0; i< keycount; i++) {
        uint64_t value = *map_get(stones, keys[i]);
        sum+=value;
    }
    printf("%lld\n",sum);
    free(keys);
    free_hashmap(stones);
}
