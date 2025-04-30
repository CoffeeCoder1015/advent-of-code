#include <assert.h>
#include <complex.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

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
    int outputs_count;
    char* outputs;
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
    o->outputs_count = 0;
    o->outputs = malloc(0);
    return o;
}

void free_operation(map_entry* e){
    operation* o = e->value;
    free(o->outputs);
    free(e->value);
}

void insert_output(operation* o, char* output){
    int index = o->outputs_count*4;
    o->outputs_count++;
    o->outputs = realloc(o->outputs, o->outputs_count*4);
    o->outputs[index] = output[0];
    o->outputs[index+1] = output[1];
    o->outputs[index+2] = output[2];
    o->outputs[index+3] = '\0';
}

void print_outputs(operation* o){
    for (int i = 0;  i < o->outputs_count; i++) {
        char* ref = &o->outputs[i*4];
        printf("%s;",ref);
    }
    printf("\n");
}

char* get_output(operation* o, int index){
    return &o->outputs[4*index];
}

void isort(char* str_array, int n){
    for (int i = 1; i < n; i++) {
        for (int j = i-1; j >= 0; j--) {
            char* jref = &str_array[j*4];
            char* jref2 = &str_array[(j+1)*4];
            if (strcmp(jref, jref2) > 0) {
                for (int k = 0; k < 3; k++) {
                    jref[k] ^= jref2[k];
                    jref2[k] ^= jref[k];
                    jref[k] ^= jref2[k];
                }
            }
        }
    }
}


// track the traversal depth
typedef struct{
    int depth;
    char start[4];
    char wire[4];
    int* op_chain;
} depth_track;

depth_track new_dt(char wire[4]){
    depth_track dt = {0,{wire[0],wire[1],wire[2],wire[3]},{wire[0],wire[1],wire[2],wire[3]},malloc(0)};
    return dt;
}

void free_dt(depth_track* dt){
    free(dt->op_chain);
}

void print_op_chain(depth_track* dt){
    char* op_map[] = {"AND","OR","XOR"};
    for (int i = 0; i < dt->depth; i++) {
        int op_code = dt->op_chain[i];
        printf("%s,",op_map[op_code]);
    }
    printf("\n");
}

depth_track increment_old(depth_track* dt, char* wire,  int op_code){
    size_t new_size = sizeof(int)*( dt->depth + 1 );
    size_t old_size = sizeof(int)*( dt->depth);
    char* s = dt->start;
    depth_track new_dt = {
        .depth = dt->depth+1,
        .start = {s[0],s[1],s[2],s[3]},
        .wire = {wire[0],wire[1],wire[2],wire[3]},
        .op_chain = malloc(new_size)
    };
    memcpy_s(new_dt.op_chain,new_size,dt->op_chain,old_size);
    new_dt.op_chain[new_dt.depth-1] = op_code;
    return new_dt;
}

typedef struct{
    int queue_cap;
    int queue_size;
    int queue_pointer;
    depth_track* array;
    void ( *free_queue )(depth_track*);
} Queue;

Queue new_queue(void ( *free_queue )(depth_track*)){
    return (Queue){10,0,0,malloc(sizeof(depth_track)*10),free_queue};
}

void free_queue(Queue* q){
    for (int i = 0;  i < q->queue_size; i++) {
        q->free_queue(&q->array[i]);
    }
    free(q->array);
}

void append_queue(Queue* q, depth_track item){
    int index = q->queue_size++;
    if (q->queue_size > q->queue_cap ) {
        q->queue_cap += 20;
        q->array = realloc(q->array, sizeof(depth_track)*q->queue_cap);
    }
    q->array[index] = item;
}

depth_track pop_queue(Queue* q){
    return q->array[q->queue_pointer++];
}

void append_str(int* array_size, char** array,char* str){
    int size = *array_size;
    char* a = *array;
    int index = size*4;
    size++;
    a = realloc(a, size*4);
    if (index > 0) {
        a[index-1]  = ',';
    }
    a[index] = str[0];
    a[index+1] = str[1];
    a[index+2] = str[2];
    a[index+3] = '\0';
    *array_size = size;
    *array = a;
}


// Basic rules
// Layer 1:
// XOR that takes in x & y 
// - must output to XOR 
// - must output to AND which takes in OR from previous adder
//
// AND that takes in x&y 
// - Take in x&y
// - must output to OR 
//
// Layer 2:
// OR
// - must take in 2 ANDs
// - > Output to a AND and a XOR
//
// AND (carry bit)
// - must take in OR from previous adder
// - must take in XOR from this adder
// - > output to a OR
//
// Layer 3:
// XOR
// - must take in XOR & OR
// - > must output to z

int main(){
    FILE* inputs;
    fopen_s(&inputs, "q24.txt", "r");

    int key_count = 0;
    char* keys = malloc(0);

    hashmap* mapping = hashmap_new(free_operation,str_to_key);

    uint64_t x = 0;
    uint64_t y = 0;
    char buffer[1024];
    for (;;) {
        char* end = fgets(buffer, 1024, inputs);
        if (buffer[0] == '\n') {
            break;
        }
        int n = strlen(buffer);
        buffer[--n] = '\0';

        char* sep = strchr(buffer, ':');
        *sep = '\0';

        char* value_str = sep+2;
        uint64_t value = atoi(value_str);

        uint64_t id = atoll(&buffer[1]);
        switch (buffer[0]) {
            case 'x': 
            x |= value << id;
            break;
            case 'y': 
            y |= value << id;
            break;
        }
        operation* ox =new_operation(NULL, NULL, -1, value);
        hashmap_set(mapping, buffer, ox);
    }
    uint64_t expected_z = x+y;
    printf("%llu+%llu=%llu\n",x,y,expected_z);

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

        int index = key_count*4;
        key_count++;
        keys = realloc(keys, key_count*4);
        keys[index] = out[0];
        keys[index+1] = out[1];
        keys[index+2] = out[2];
        keys[index+3] = '\0';
    }
    fclose(inputs);
    isort(keys,key_count);

    for (int i = 0;  i < key_count; i++) {
        char* key = &keys[i*4];
        operation* o = hashmap_get(mapping, key).value;
        operation* o1  = hashmap_get(mapping, o->input1).value;
        operation* o2  = hashmap_get(mapping, o->input2).value;
        insert_output(o1, key);
        insert_output(o2, key);
    }

    hashmap* visited = hashmap_new(NULL, str_to_key);

    int wrong_count = 0;
    char* wrong_wires = malloc(0);

    for (int i = 0;  i < key_count; i++) {
        char* key = &keys[i*4];
        operation* o = hashmap_get(mapping, key).value;
        // result check = hashmap_get(visited, key);
        // XOR analysis
        // 1.
        // if is of form A XOR B -> C
        // and A not x or y
        // and B not x or y
        // and c not z
        //
        // 2. 
        // if C is z and not of form
        // A OP B -> C, where OP is not XOR
        // except for z45 which is a carry bit
        //
        // 3. 
        // XOR never outputs to OR
        bool isz = key[0] == 'z';
        bool failed = false;
        if (o->op_code == 2) {
            bool first_valid = 'x' == o->input1[0]  || 'y' == o->input1[0];
            bool second_valid = 'x' == o->input2[0]  || 'y' == o->input2[0];
            failed = !first_valid && !second_valid && !isz;

            for (int j = 0; j < o->outputs_count; j++) {
                char* sub_wire_key = get_output(o, j);
                operation* sub_wire = hashmap_get(mapping, sub_wire_key).value;
                if (sub_wire->op_code == 1) {
                    failed = true; 
                    break;
                }
            }
        }else {
            failed = isz && strcmp("z45", key);
        }

        // AND analysis
        // must output to OR
        // except for the AND from the 0th bit 
        // which will go directly into the next adder
        bool from_0th_bit = strcmp(o->input1, "x00") == 0 || strcmp(o->input2, "x00") == 0;
        if (o->op_code == 0  && !from_0th_bit) {
            for (int j = 0; j < o->outputs_count; j++) {
                char* wire = get_output(o, j);
                operation* sub_wire = hashmap_get(mapping, wire).value;
                if ( sub_wire->op_code != 1){
                    failed = true;
                    break;
                }
            }
        }

        if (failed) {
            append_str(&wrong_count, &wrong_wires, key) ;
        }
    }
    printf("%s\n",wrong_wires);
    hashmap_free(visited);
    hashmap_free(mapping);
}
