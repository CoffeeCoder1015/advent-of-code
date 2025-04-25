#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    int length;
    int8_t* bytes;
} Key;

typedef struct{
    Key key;
    void* value;
} map_entry;

typedef struct{
    map_entry* entries;
    int length;
} map_slot;

typedef struct{
    map_slot* slots;
    int length;
    int capacity;
    //used to free the indivual values in kv pairs
    //set to NULL if void* value does not actually point to a heap allocated resource
    void (*free_entry)(map_entry*);
    // used to convert a general datatype into a Key object
    Key (*to_key)(void*);
} hashmap;

#define INIT_CAPACITY 6400
hashmap* hashmap_new(void (*free_function)(map_entry*), Key (*to_key)(void*)){
    hashmap* h = malloc(sizeof(hashmap));
    h->length = 0;
    h->capacity = INIT_CAPACITY;
    h->free_entry = free_function;
    h->to_key = to_key;

    h->slots = calloc(h->capacity, sizeof(map_slot));
    return h;
}

void hashmap_free(hashmap* h){
    // for hashmaps with heap allocated structs
    if (h->free_entry != NULL) {
        for (int i = 0; i < h->capacity; i++) {
            map_slot* entry_ptr = &h->slots[i];
            bool is_empty_entry = entry_ptr->length == 0;
            if (!is_empty_entry) {
                for (int entry = 0; entry < entry_ptr->length; entry++) {
                    h->free_entry(&entry_ptr->entries[entry]);
                    Key k = entry_ptr->entries[entry].key;
                    free(k.bytes);
                }
                free(entry_ptr->entries);
            }
        }
        free(h->slots);
        free(h);
        return;
    }
    // for hashmaps with primitive values stored as cast (void*) pointers
    for (int i = 0; i < h->capacity; i++) {
        map_slot* entry_ptr = &h->slots[i];
        bool is_empty_entry = entry_ptr->length == 0;
        if (!is_empty_entry) {
            for (int entry = 0; entry < entry_ptr->length; entry++) {
                Key k = entry_ptr->entries[entry].key;
                free(k.bytes);
            }
            free(entry_ptr->entries);
        }
    }
    free(h->slots);
    free(h);
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
uint64_t Key_hash(Key key){
    uint64_t hash = FNV_OFFSET;
    for (int i= 0; i < key.length; i++) {
        uint8_t byte_of_data = key.bytes[i];
        hash *= FNV_PRIME;
        hash ^= byte_of_data;
    }
    return hash;
}

Key new_key(int bytes){
    Key k = { bytes, malloc(sizeof(int8_t)*bytes) };
    return k;
}

bool compare_key(Key key1, Key key2){
    if (key1.length != key2.length) {
        return false;
    }
    for (int i = 0; i < key1.length; i++) {
        if (key1.bytes[i]!=key2.bytes[i]) {
            return false;
        }
    }
    return true;
}

typedef struct{
    bool found;
    void* value;
} result;

result hashmap_get(hashmap* h, void* key){
    Key converted_key = h->to_key(key);
    uint64_t hash = Key_hash(converted_key);

    size_t index = (size_t)(hash & (uint64_t)(h->capacity - 1));

    map_slot hashed_position = h->slots[index];

    for (int i = 0; i < hashed_position.length; i++) {
        map_entry item =  hashed_position.entries[i];
        if (compare_key(converted_key, item.key)) {
            free(converted_key.bytes);
            return ( result ) {true,item.value};
        }
    }

    free(converted_key.bytes);
    return (result){false,NULL};
}
//returns true if expansion is succsessful
//returns false if expansion fails
bool hashmap_expand(hashmap* h){
    size_t new_capacity = h->capacity * 2;
    map_slot* new_entries = calloc(new_capacity, sizeof(map_slot));
    if (new_entries == NULL) {
        return false; 
    }
    for (int i = 0 ; i < h->capacity; i++) {
        map_slot item = h->slots[i];
        for (int j = 0; j < item.length; j++) {
            uint64_t hash = Key_hash(item.entries[j].key);

            size_t index = (size_t)(hash & (uint64_t)(new_capacity - 1));

            map_slot* hashed_position = &new_entries[index];

            hashed_position->length++;
            if (hashed_position->length == 1) {
                hashed_position->entries = malloc(sizeof(map_entry));
            }else {
                hashed_position->entries = realloc(hashed_position->entries, sizeof(map_entry)*hashed_position->length);
            }

            hashed_position->entries[hashed_position->length-1] = item.entries[j];
        }
    }

    for (int i = 0; i < h->capacity; i++) {
        map_slot* entry_ptr = &h->slots[i];
        bool is_empty_entry = entry_ptr->length == 0;
        if (!is_empty_entry) {
            free(entry_ptr->entries);
        }
    }
    free(h->slots);
    h->slots = new_entries;
    h->capacity = new_capacity;
    return true;

}

bool hashmap_set(hashmap* h, void* key, void* value){
    if (h->length >= h->capacity / 2) {
        if (!hashmap_expand(h)) {
            return false; 
        }
    }

    Key converted_key = h->to_key(key);
    uint64_t hash = Key_hash(converted_key);

    size_t index = (size_t)(hash & (uint64_t)(h->capacity - 1));

    map_slot* hashed_position = &h->slots[index];
    for (int i = 0; i < hashed_position->length; i++) {
        map_entry* item = &hashed_position->entries[i];
        if (compare_key(converted_key, item->key)) {
            item->value = value;
            free(converted_key.bytes);
            return true;
        }    
    }

    //if key is not found
    h->length++;
    map_entry item = {converted_key,value};
    hashed_position->length++;
    if (hashed_position->length == 1) {
        hashed_position->entries = malloc(sizeof(map_entry));
    }else {
        hashed_position->entries = realloc(hashed_position->entries, sizeof(map_entry)*hashed_position->length);
    }

    hashed_position->entries[hashed_position->length-1] = item;
    return true;
}

Key seq_to_key(void* gen_seq){
    int* seq = gen_seq;
    Key k = new_key(4);
    for (int i = 0; i < 4; i++) {
        k.bytes[i] = seq[i];
    }
    return k;
}


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
    int8_t* store_array = malloc(sizeof(int8_t)*2000*line_count);

    l_end = strchr(buffer, '\n');
    char* l_start = buffer;
    while (l_end != NULL) {
        *l_end = '\0';
        size_t sec = atoll(l_start);
        for (int i = 0; i < 2000; i++) {
            int8_t price = sec%10;
            store_array[i+loaded*2000] = price;
            sec = gen_sec_num(sec);
        }
        loaded++;

        l_start = l_end+1;
        l_end = strchr(l_end+1, '\n');
    }
    free(buffer);

    hashmap* s = hashmap_new(NULL, seq_to_key);

    int kc = 0;
    int (*keys)[4] = malloc(0);
    for (int i = 0; i < line_count; i++) {
        hashmap* repeat = hashmap_new(NULL, seq_to_key);
        for (int j = 4+i*2000; j < 2000+i*2000; j++) {
            int diff1 = store_array[j-3]-store_array[j-4];
            int diff2 = store_array[j-2]-store_array[j-3];
            int diff4 = store_array[j]-store_array[j-1];
            int diff3 = store_array[j-1]-store_array[j-2];
            int key[] = {diff1,diff2,diff3,diff4};

            result check = hashmap_get(repeat, key);
            if (!check.found) {
                hashmap_set(repeat, key, (void*)1);
                result r = hashmap_get(s, key);
                if (r.found) {
                    size_t old = (uint64_t)r.value; 
                    size_t new = old + store_array[j];
                    hashmap_set(s,key,(void*)new);
                }else {
                    kc++;
                    keys = realloc(keys, sizeof(int[4])*kc);
                    keys[kc-1][0]  = key[0];
                    keys[kc-1][1]  = key[1];
                    keys[kc-1][2]  = key[2];
                    keys[kc-1][3]  = key[3];

                    hashmap_set(s,key,(void*)(uint64_t)store_array[j]);
                }
            }
        }
        hashmap_free(repeat);
    }

    int current_highest = 0;
    for (int i = 0; i < kc; i++) {
        int* key = keys[i];
        result r = hashmap_get(s, key);
        if (r.found) {
            int max = (int)(size_t)r.value; 
            if (max > current_highest) {
                current_highest = max; 
            }
        }
    }
    printf("%d\n",current_highest);
    hashmap_free(s);
    free(keys);
    free(store_array);
}
