#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    int weight;
    int pos[2];
} mhitem;

typedef struct{
    int heap_length;
    mhitem* heap_array;
} minheap;


minheap new_minheap(){
    minheap new_heap;
    new_heap.heap_length = 0;
    new_heap.heap_array = malloc(0);
    return new_heap;
}

void free_minheap(minheap* mh){
    free( mh->heap_array );
}

void minheap_swap(minheap* mh, int index1, int index2){
    mhitem temp = mh->heap_array[index1];
    mh->heap_array[index1] = mh->heap_array[index2];
    mh->heap_array[index2] = temp;
}

void minheap_heapify(minheap* mh,int index){
    if (mh->heap_length < 2) {
        return; 
    }
    for (;;) {
        int left_child = 2*index+1;
        int right_child = 2*index+2;

        int smallest = index;
        bool has_space_left = left_child < mh->heap_length;
        bool has_space_right = right_child < mh->heap_length;
        if (has_space_left && mh->heap_array[left_child].weight < mh->heap_array[smallest].weight) {
            smallest = left_child; 
        }
        if (has_space_right && mh->heap_array[right_child].weight < mh->heap_array[smallest].weight) {
            smallest = right_child; 
        }

        if (smallest != index) {
            minheap_swap(mh,smallest,index);
            index = smallest;
        }else {
            break; 
        }
    }
}

void minheap_insert(minheap* mh, mhitem item){
    mh->heap_length++;
    mh->heap_array = realloc(mh->heap_array, sizeof(mhitem)*mh->heap_length);
    mh->heap_array[mh->heap_length-1] = item;
    for (int i = mh->heap_length-1; i > 0;) {
        int i_parent = (i-2)/2;
        mhitem current = mh->heap_array[i];
        mhitem parent = mh->heap_array[i_parent];
        if (current.weight < parent.weight) {
            minheap_swap(mh, i,i_parent);
        }else{
            break;
        }
        i = i_parent;
    }
}

void minheap_delete(minheap* mh){
    if (mh->heap_length == 0) {
        return; 
    }
    mhitem last = mh->heap_array[mh->heap_length-1];
    mh->heap_array[0] = last;
    mh->heap_length--;
    minheap_heapify(mh, 0);
}

mhitem minheap_extract(minheap* mh){
    mhitem item = mh->heap_array[0];
    minheap_delete(mh);
    return item;
}

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

Key pos_to_key(void* pos_generic){
    int32_t* pos = pos_generic;
    Key k = new_key(8);

    uint64_t combined = (uint64_t)pos[0] << 32;
    combined |= pos[1];
    for (int i = 0;  i < 8; i++) {
        uint8_t byte_of_data = ( combined >> (i * 8) ) & 255; // grabs 8 bit slices out of the data
        k.bytes[i] = byte_of_data;
    }
    return k;
}

int main(){
    FILE* input;
    fopen_s(&input, "test.txt", "rb");

    fseek(input, 0, SEEK_END);
    size_t file_size = ftell(input);
    rewind(input);

    char* map = malloc(file_size);
    fread(map, sizeof(char) , file_size, input);
    map[file_size] = '\0';
    fclose(input);

    char* line_end = strchr(map, '\n');
    int x_size = line_end-map;
    int y_size = 0;
    int start_pos[2] = {0,0};
    int end_pos[2] = {0,0};
    while (line_end != NULL) {
        y_size++; 
        char* s_char = strchr(line_end, 'S');
        if (s_char != NULL) {
            start_pos[0] = s_char-line_end-1;
            start_pos[1] = y_size;
        }
        char* e_char = strchr(line_end, 'E');
        if (e_char != NULL) {
            end_pos[0] = e_char-line_end-1;
            end_pos[1] = y_size;
        }
        line_end = strchr(line_end+1,'\n');
    }
    printf("%d,%d %d,%d Size:%d,%d\n",start_pos[0],start_pos[1],end_pos[0],end_pos[1],x_size,y_size);

    // Pathfinding setup
    minheap pq = new_minheap();
    hashmap* distances = hashmap_new(NULL,pos_to_key);

    // Inserting inital values
    mhitem start = {0,{start_pos[0],start_pos[1]}};
    minheap_insert(&pq, start);

    hashmap_set(distances, start_pos, 0);


    printf("%s\n",map);

    free(map);
}
