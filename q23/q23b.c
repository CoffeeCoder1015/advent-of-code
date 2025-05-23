#include <assert.h>
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
    int conn_count;
    char* contig_store;
} conn_list;

conn_list* new_connection(){
    conn_list* c = malloc(sizeof(conn_list));
    c->conn_count = 0;
    c->contig_store = malloc(0);
    return c;
}

void free_conn_list(conn_list* c){
    free(c->contig_store);
    free(c);
}

void free_conn_list_hashmap(map_entry* e){
    conn_list* c = e->value;
    free(c->contig_store);
    free(e->value);
}

void append_connection(conn_list* c, char* new_computer){
    int current_str_len = c->conn_count*3;
    int new_len = current_str_len+3;
    c->conn_count++;
    c->contig_store = realloc(c->contig_store,new_len);
    c->contig_store[current_str_len] = new_computer[0];
    c->contig_store[1+current_str_len] = new_computer[1];
    c->contig_store[2+current_str_len] = '\0';
}

char* get_connection(conn_list* c, int index){
    return &c->contig_store[index*3];
}

void print_connections(conn_list* c){
    int n = c->conn_count;
    for (int i = 0; i < n; i++) {
        printf("c-> %s; ",get_connection(c, i));
    }
    printf("\n");
}

typedef struct {
    int stacklen;
    int stackcap;
    void** stack_items;
    void (*free_func)(void*);
} Stack;

Stack new_stack(void (*free_func)(void*)){
    Stack s = { 0, 10, malloc(sizeof(void*)*10), free_func };
    return s;
}

void free_stack(Stack* s){
    for (int i = 0; i < s->stacklen; i++) {
        s->free_func(s->stack_items[i]);
    }
    free(s->stack_items);
}

void stack_append(Stack* s, void* item){
    s->stacklen++;
    if (s->stacklen>s->stackcap) {
        s->stackcap+=20; 
        s->stack_items = realloc(s->stack_items, sizeof(void*)*s->stackcap);
    }
    s->stack_items[s->stacklen-1] = item;
}

void* stack_pop(Stack* s){
    s->stacklen--;
    return s->stack_items[s->stacklen];
}

typedef struct{
    int depth;
    char computer[3];
    char* computers;
} depth_track;

depth_track* new_dt(char* computer){
    depth_track* dt = malloc(sizeof(depth_track));
    dt->depth = 1;
    dt->computers = malloc(0);
    dt->computer[0] = computer[0];
    dt->computer[1] = computer[1];
    dt->computer[2] = '\0';
    return dt;
}

void free_dt(void* items){
    depth_track* dt = items;
    free(dt->computers);
    free(dt);
}

// duplicates old one so a new one can be used in the stack
// doesnt delete the old one because that will get handled by the stack
// when all of it get freed
depth_track* dt_increment_old(depth_track* old_dt,char* new_computer){
    depth_track* new_dt = malloc(sizeof(depth_track));
    memcpy_s(new_dt, sizeof(depth_track),old_dt , sizeof(depth_track));
    int n = 3*( new_dt->depth-1 );
    new_dt->computers = malloc(new_dt->depth*3);
    strcpy_s(new_dt->computers, new_dt->depth*3, old_dt->computers);
    if (n > 0) {
        new_dt->computers[n-1] = ',';
    }
    new_dt->computers[n] = new_dt->computer[0];
    new_dt->computers[n+1] = new_dt->computer[1];
    new_dt->computers[n+2] = new_dt->computer[2];
    new_dt->depth++;
    new_dt->computer[0] = new_computer[0];
    new_dt->computer[1] = new_computer[1];
    new_dt->computer[2] = '\0';
    return new_dt;
}

int main(){
    FILE* input;
    fopen_s(&input, "q23.txt", "rb");

    hashmap* connections = hashmap_new(free_conn_list_hashmap,str_to_key);

    int cc = 0;
    char* Computers = malloc(0);
    for (;;) {
        char buffer[1024];
        char* end = fgets(buffer,1024,input);
        if (end == NULL) {
            break; 
        }
        int n = strlen(buffer);
        buffer[--n] = '\0';
        buffer[2] = '\0';
        char* c1 = &buffer[0];
        char* c2 = &buffer[3];

        if (c1[0] > c2[0]) {
            char* t = c2;
            c2 = c1;
            c1 = t;
        }else if (c1[0] == c2[0]) {
            if (c1[1] > c2[1]) {
                char* t = c2;
                c2 = c1;
                c1 = t;
            }
        }
        result r = hashmap_get(connections, c1);
        if (r.found) {
            append_connection(r.value, c2);
        }else {
            cc+=2; 
            Computers = realloc(Computers, cc);
            Computers[cc-2] = c1[0];
            Computers[cc-1] = c1[1];

            conn_list* nc = new_connection();
            append_connection(nc, c2);
            hashmap_set(connections,c1,nc);
        }
    }
    fclose(input);

    // General loop tracking:
    //
    // At traversal depth n, one of the keys must match with one of the keys at traversal depth 1
    // to form a loop of length n;
    //
    // This requires:
    // 1. Depth tracked traversal: A queue or stack that takes in a struct which will have a depth property.
    // 2. re-comparing the items at traversal depth 1 to the oens at traversal depth n

    int highest_depth = 1;
    char* seq = malloc(0);
    for (int i = 0; i < cc; i+=2) {
        char key[] = {Computers[i],Computers[i+1],'\0'};
        Stack s = new_stack(free_dt);

        result root = hashmap_get(connections, key);
        if (root.found) {
            conn_list* c = root.value;

            stack_append(&s,new_dt(key));

            while (s.stacklen > 0) {
                depth_track* dt = (depth_track*)stack_pop(&s);
                char* next_key = dt->computer;
                result r = hashmap_get(connections, next_key);

                int connections_made = 0;
                for (int j = 0; j < dt->depth-1; j++) {
                    char key[3];
                    key[2] = '\0';
                    key[0] = dt->computers[j*3];
                    key[1] = dt->computers[j*3+1];
                    result r = hashmap_get(connections,key);
                    if (r.found) {
                        conn_list* cls = r.value; 
                        for (int k = 0; k < cls->conn_count; k++) {
                            char* checking_computer = get_connection(cls, k);
                            bool isIn = strcmp(checking_computer, next_key) == 0;
                            if (isIn) {
                                connections_made++;
                                break;  
                            }
                        }
                    }
                }
                if (connections_made+1 == dt->depth) {
                    if (dt->depth > highest_depth) {
                        // set answer in answer region;
                        highest_depth = dt->depth;
                        seq = realloc(seq, highest_depth*3);
                        strcpy_s(seq, highest_depth*3, dt->computers);
                        int n = ( dt->depth-1 )*3;
                        if (n > 0) {
                            seq[n-1] = ',';
                        }
                        seq[n] = dt->computer[0];
                        seq[n+1] = dt->computer[1];
                        seq[n+2] = dt->computer[2];
                    }
                }else {
                    continue;
                }
                if (r.found) {
                    conn_list* layer_x = r.value;
                    for (int j = layer_x->conn_count-1; j >= 0; j--) {
                        char* computer = get_connection(layer_x, j);
                        stack_append(&s, dt_increment_old(dt, computer));
                    } 
                }
                free_dt(dt);
            }
        }

        free_stack(&s);
    }
    printf("%s\n",seq);
    free(seq);
    hashmap_free(connections);
    free(Computers);
}
