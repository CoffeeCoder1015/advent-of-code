#include <stdint.h>
#include <stdbool.h>
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
} Map_KV;

typedef struct{
    Map_KV* collisions;
    int length;
} Map_hashed_position;

typedef struct{
    Map_hashed_position* map_entries;
    int length;
    int capacity;
    //used to free the indivual values in kv pairs
    //set to NULL if void* value does not actually point to a heap allocated resource
    void (*free_entry)(Map_KV*);
    // used to convert a general datatype into a Key object
    Key (*to_key)(void*);
} hashmap;

#define INIT_CAPACITY 6400
hashmap* hashmap_new(void (*free_function)(Map_KV*), Key (*to_key)(void*)){
    hashmap* h = malloc(sizeof(hashmap));
    h->length = 0;
    h->capacity = INIT_CAPACITY;
    h->free_entry = free_function;
    h->to_key = to_key;

    h->map_entries = calloc(h->capacity, sizeof(Map_hashed_position));
    return h;
}

void hashmap_free(hashmap* h){
    // for hashmaps with heap allocated structs
    if (h->free_entry != NULL) {
        for (int i = 0; i < h->capacity; i++) {
            Map_hashed_position* entry_ptr = &h->map_entries[i];
            bool is_empty_entry = entry_ptr->length == 0;
            if (!is_empty_entry) {
                for (int entry = 0; entry < entry_ptr->length; entry++) {
                    h->free_entry(&entry_ptr->collisions[entry]);
                    Key k = entry_ptr->collisions[entry].key;
                    free(k.bytes);
                }
                free(entry_ptr->collisions);
            }
        }
        return;
    }
    // for hashmaps with primitive values stored as cast (void*) pointers
    for (int i = 0; i < h->capacity; i++) {
        Map_hashed_position* entry_ptr = &h->map_entries[i];
        bool is_empty_entry = entry_ptr->length == 0;
        if (!is_empty_entry) {
            for (int entry = 0; entry < entry_ptr->length; entry++) {
                Key k = entry_ptr->collisions[entry].key;
                free(k.bytes);
            }
            free(entry_ptr->collisions);
        }
    }
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

    Map_hashed_position hashed_position = h->map_entries[index];

    for (int i = 0; i < hashed_position.length; i++) {
        Map_KV item =  hashed_position.collisions[i];
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
    Map_hashed_position* new_entries = calloc(new_capacity, sizeof(Map_hashed_position));
    if (new_entries == NULL) {
        return false; 
    }
    for (int i = 0 ; i < h->capacity; i++) {
        Map_hashed_position item = h->map_entries[i];
        for (int j = 0; j < item.length; j++) {
            uint64_t hash = Key_hash(item.collisions[j].key);

            size_t index = (size_t)(hash & (uint64_t)(new_capacity - 1));

            Map_hashed_position* hased_position = &new_entries[index];

            hased_position->length++;
            if (hased_position->length == 1) {
                hased_position->collisions = malloc(sizeof(Map_KV));
            }else {
                hased_position->collisions = realloc(hased_position->collisions, sizeof(Map_KV)*hased_position->length);
            }

            hased_position->collisions[hased_position->length-1] = item.collisions[j];
        }
    }

    free(h->map_entries);
    h->map_entries = new_entries;
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

    Map_hashed_position* hased_position = &h->map_entries[index];
    for (int i = 0; i < hased_position->length; i++) {
        Map_KV* item = &hased_position->collisions[i];
        if (compare_key(converted_key, item->key)) {
            item->value = value;
            free(converted_key.bytes);
            return true;
        }    
    }

    //if key is not found
    h->length++;
    Map_KV item = {converted_key,value};
    hased_position->length++;
    if (hased_position->length == 1) {
        hased_position->collisions = malloc(sizeof(Map_KV));
    }else {
        hased_position->collisions = realloc(hased_position->collisions, sizeof(Map_KV)*hased_position->length);
    }

    hased_position->collisions[hased_position->length-1] = item;
    return true;
}

#define TESTING
#ifdef TESTING
    #define SQR_SIZE 7
    #define INPUT "test.txt"
    #define SIM 12
#else
    #define SQR_SIZE 71
    #define INPUT "q18.txt"
    #define SIM 1024
#endif

int square_size = SQR_SIZE;
char gridAtlas[] = ".#O";

void printGrid(int* grid){
    for (int i = 0; i < square_size*square_size; i++) {
        printf("%c",gridAtlas[grid[i]]);
        if ((i+1)%square_size == 0) {
            printf("\n");
        }
    }
}

int main(){
    FILE* input;
    fopen_s(&input,INPUT,"rb");

    fseek(input, 0, SEEK_END);
    size_t read_count = ftell(input);
    rewind(input);

    char* raw_coords = malloc(read_count+1);
    raw_coords[read_count] = '\0';
    fread(raw_coords, sizeof(char), read_count, input);
    fclose(input);

    int* memory = calloc(square_size*square_size, sizeof(int));


    char* line_end = strchr(raw_coords, '\n');
    char* line_start = raw_coords;
    for (int i = 0; i < SIM; i++) {
        char* sep = strchr(line_start, ',');
        *sep = '\0';
        int x = atoi(line_start);
        *line_end = '\0';
        int y = atoi(sep+1);

        memory[y*(square_size)+x] = 1;

        if (line_end != NULL) {
            line_start = line_end+1;
            line_end = strchr(line_end+1, '\n');
        }
    }
    printGrid(memory);
    free(raw_coords);
}
