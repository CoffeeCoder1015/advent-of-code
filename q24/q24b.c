#include <assert.h>
#include <complex.h>
#include <stdbool.h>
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

Key str_to_key(void* strgeneric){
    char* string = strgeneric;
    int n = strlen(string);
    Key k = new_key(n);
    for (int i = 0; i < n; i++) {
        k.bytes[i] = string[i];
    }
    return k;
}

typedef struct{
    char input1[4];
    char input2[4];
    int op_code;
    // for preset values, only output will have a value;
    // and for all other, output will be -1, indicating that
    int output;
} operation;

operation* new_operation(char* input1, char* input2, int op_code, int output){
    operation* o = malloc(sizeof(operation));
    strcpy_s(o->input1, 4, input1);
    strcpy_s(o->input2, 4, input2);
    o->op_code = op_code;
    o->output = output;
    return o;
}

void free_operation(map_entry* e){
    free(e->value);
}

void z_isort(char* str_array, int n){
    for (int i = 1; i < n; i++) {
        char* ref = &str_array[i*4];
        int refn = atoi(&ref[1]);
        for (int j = i-1; j >= 0; j--) {
            char* jref = &str_array[j*4];
            int jrefn = atoi(&jref[1]);
            char* jref2 = &str_array[(j+1)*4];
            int jrefn2 = atoi(&jref2[1]);
            if (jrefn < jrefn2) {
                for (int k = 0; k < 3; k++) {
                    jref[k] ^= jref2[k];
                    jref2[k] ^= jref[k];
                    jref[k] ^= jref2[k];
                }
            }
        }
    }
}

int main(){
    FILE* inputs;
    fopen_s(&inputs, "q24.txt", "r");

    hashmap* mapping = hashmap_new(free_operation,str_to_key);

    char buffer[1024];
    for (;;) {
        char* end = fgets(buffer, 1024, inputs);
        if (buffer[0] == '\n') {
            break;;
        }
        int n = strlen(buffer);
        buffer[--n] = '\0';

        char* sep = strchr(buffer, ':');
        *sep = '\0';

        char* value_str = sep+2;
        int value = atoi(value_str);

        operation* ox =new_operation(NULL, NULL, -1, value);
        hashmap_set(mapping, buffer, ox);
    }

    int z_count = 0;
    char* z_keys = malloc(0);
    for (;;) {
        char* end = fgets(buffer, 1024, inputs);
        if (end == NULL) {
            break; 
        }
        int n = strlen(buffer);
        buffer[--n] = '\0';
        char* sep = strchr(buffer, ' ');
        *sep = '\0';
        char* i1 = buffer;
        char* sep2 = strchr(sep+1, ' ');
        *sep2 = '\0';
        char* op = sep+1;
        char* sep3 = strchr(sep2+1,  ' ');
        *sep3 = '\0';
        char* i2 = sep2+1;
        char* out = strchr(sep3+1, '>')+2;
        int opcode = 0;
        switch (op[0]) {
            case 'A': // AND
                opcode = 0;
            break;
            case 'O': // OR
                opcode = 1;
            break;
            case 'X': // XOR
                opcode = 2;
            break;
        }
        operation* ox = new_operation(i1, i2, opcode, -1);
        hashmap_set(mapping,out,ox);

        if (out[0] == 'z') {
            z_count++; 
            z_keys = realloc(z_keys, 4*z_count);
            int ref = 4*(z_count-1);
            z_keys[ref] = out[0];
            z_keys[ref+1] = out[1];
            z_keys[ref+2] = out[2];
            z_keys[ref+3] = '\0';
        }
    }
    fclose(inputs);
    z_isort(z_keys, z_count);

    uint64_t answer = 0;
    for (int i = 0; i < z_count ; i++) {
        char* ref = &z_keys[i*4];

        int stack_size = 1;
        char* call_stack = malloc(4);
        call_stack[0] = '\0';
        strcat_s(call_stack, 4, ref);

        while (stack_size > 0) {
            int front_ref = 4*( stack_size-1 );
            char* front = &call_stack[front_ref];
            result r = hashmap_get(mapping, front);
            if (r.found) {
                operation* opx = r.value; 
                // check if self has a filled output
                if (opx->output != -1) {
                    stack_size--;
                    continue; 
                }
                // check for input 1 for a filled output
                result i1 = hashmap_get(mapping, opx->input1);
                int i1v = -1;
                if (i1.found) {
                    operation* i1_op = i1.value;         
                    if (i1_op->output == -1){
                        stack_size++;
                        call_stack = realloc(call_stack, 4*stack_size);
                        int ref = 4*(stack_size-1);
                        call_stack[ref] = opx->input1[0];
                        call_stack[ref+1] = opx->input1[1];
                        call_stack[ref+2] = opx->input1[2];
                        call_stack[ref+3] = '\0';
                        continue;
                    }
                    i1v = i1_op->output;
                }
                // check for input 2 for a filled output
                result i2 = hashmap_get(mapping, opx->input2);
                int i2v = -1;
                if (i2.found) {
                    operation* i2_op = i2.value;         
                    if (i2_op->output == -1){
                        stack_size++;
                        call_stack = realloc(call_stack, 4*stack_size);
                        int ref = 4*(stack_size-1);
                        call_stack[ref] = opx->input2[0];
                        call_stack[ref+1] = opx->input2[1];
                        call_stack[ref+2] = opx->input2[2];
                        call_stack[ref+3] = '\0';
                        continue;
                    }
                    i2v = i2_op->output;
                }
                // set output value
                switch (opx->op_code) {
                    case 0:
                        opx->output = i1v & i2v;
                    break;
                    case 1:
                        opx->output = i1v | i2v;
                    break;
                    case 2:
                        opx->output = i1v ^ i2v;
                    break;
                }
            }
        }

        free(call_stack);

        result r = hashmap_get(mapping, ref);
        if (r.found) {
            operation* of = r.value;
            int shift_count = z_count-i-1;
            uint64_t formatted_leading = of->output;
            formatted_leading <<= shift_count;
            answer ^= formatted_leading;  
        }
    }
    printf("%llu\n",answer);

    free(z_keys);
    hashmap_free(mapping);
}
