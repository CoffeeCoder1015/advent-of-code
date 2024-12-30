#include <corecrt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// referncing https://web.stanford.edu/class/archive/cs/cs161/cs161.1168/lecture4.pdf
// implementing heap insertion, heap deletion 
// heap inc/dec key is also not needed
// heap extractkey is just a compound action of get array[0] and dropping array[0]
// and reheapiydying
//

typedef struct{
    int potential;
    int pos[2];
} mheap_item;

typedef struct{
    mheap_item* heap;
    int length;
} minheap;

void check_heap(minheap* mh){
    int length = mh->length;
    bool good = true;
    for (int i = 0;i < length; i++) {
        mheap_item current_node = mh->heap[i];
        int current_potential = current_node.potential;
        int child1_index = 2*i;
        int child2_index = child1_index+1;
        if (child1_index < length) {
            int child_potential = mh->heap[child1_index].potential;
            if (current_potential > child_potential) {
                good = false ;
                printf("Failed at node [%d]: Parent=%d Child1=%d\n",i,current_potential,child_potential);
            } 
        }
        if (child2_index < length) {
            int child_potential = mh->heap[child2_index].potential;
            if (current_potential > child_potential) {
                good = false ;
                printf("Failed at node [%d]: Parent=%d Child2=%d\n",i,current_potential,child_potential);
            } 
        }
    }
    printf("Heap is %s\n",good? "good" : "bad") ;
}

minheap new_minheap(){
    minheap new_heap;
    new_heap.length = 0;
    new_heap.heap = malloc(0);
    return new_heap;
}

void free_minheap(minheap* mh){
    free( mh->heap );
}

void minheap_swap_index(minheap* mh, int index1, int index2){
    mheap_item temp = mh->heap[index1];
    mh->heap[index1] = mh->heap[index2];
    mh->heap[index2] = temp;
}

// for a heap node in an array at index i
// a left child node would be at 2i
// a right child node would be at 2i+1
// a parent node would be at floor(i/2)
void minheap_insert(minheap* mh, mheap_item item){
    mh->length++;
    int length = mh->length;
    mh->heap = realloc(mh->heap, sizeof(mheap_item)*length);
    mh->heap[length-1] = item;
    // reorder heap
    for (int i = length-1; i > 0; i = (i-1)/2) {
        mheap_item current = mh->heap[i];
        mheap_item parent = mh->heap[i/2];
        if (parent.potential > current.potential) {
            minheap_swap_index(mh,i , i/2);
        }else {
            break;
        }
    }
}

void minheap_heapify(minheap* mh, int index){
    int length = mh->length;
    if (length <= 1) {
        return; 
    }
    int start_index = index;
    for (;;) {
        int left_index = 2*start_index+1;
        int right_index = left_index+2;

        int smallest_index = start_index;
        if (left_index < length && mh->heap[left_index].potential < mh->heap[smallest_index].potential) {
            smallest_index = left_index; 
        }

        if (right_index < length &&  mh->heap[right_index].potential < mh->heap[smallest_index].potential ) {
            smallest_index = right_index; 
        }
        if (smallest_index != start_index) {
            minheap_swap_index(mh,start_index,smallest_index);
            start_index = smallest_index;
        }else {
            break;
        }
    }
}

void minheap_delte(minheap* mh){
    int length = mh->length;
    mheap_item last = mh->heap[length-1];
    mh->heap[0] = last;
    mh->length--;
    length--;
    minheap_heapify(mh, 0);
}

mheap_item minheap_extract(minheap* mh){
    mheap_item item = mh->heap[0];
    minheap_delte(mh);
    return item;
}


typedef struct{
    uint64_t key;
    void* value;
} map_entry;

typedef struct{
    size_t length;
    map_entry* kv_paris;
} map_slot;

typedef struct{
    map_slot *entries;
    size_t capacity;    // size of _entries array
    size_t length;  // number of items in hash table
    //used to free the indivual values in kv pairs
    //set to NULL if void* value does not actually point to a heap allocated resource
    void (*free_entry)(map_entry*);
} hashmap;

#define INIT_CAPACITY 6400
hashmap* new_hashmap(void (*free_function)(map_entry*)){
    hashmap* h = malloc(sizeof(hashmap));
    h->length = 0;
    h->capacity = INIT_CAPACITY;
    h->free_entry = free_function;

    h->entries = calloc(h->capacity, sizeof(map_slot));
    return h;
}

void free_hashmap(hashmap* h){
    for (size_t i = 0; i < h->capacity; i++) {
        if (h->entries[i].length > 0) {
            if (h->free_entry != NULL) {
                for (size_t j = 0; j < h->entries[i].length; j++) {
                    h->free_entry(&h->entries[i].kv_paris[j]);
                }
            }
            free(h->entries[i].kv_paris);
        }
    }
    free(h->entries);
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

uint64_t pos_to_key(int32_t pos[2]){
    uint64_t key = (uint64_t)pos[0] << 32;
    key |= pos[1];
    return key;
}

uint64_t* map_set_entry(map_slot* entries, size_t capacity, size_t* plength, uint64_t key, void* value) {
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

    for (size_t i = 0; i < entries[index].length; i++) {
        map_entry* current_entry = &entries[index].kv_paris[i];
        if (current_entry->key == key) {
            //update value
            current_entry->value = value;
            return &current_entry->key;
        }
    }
    //if key wasnt found
    if (plength != NULL) {
        ( *plength )++; 
    }
    map_entry entry = {key,value};
    entries[index].length++;
    if (entries[index].length == 1) {
        entries[index].kv_paris = malloc(sizeof(map_entry));
    }else {
        entries[index].kv_paris = realloc(entries[index].kv_paris, sizeof(map_entry)*entries[index].length);
    }
    map_entry* last = &entries[index].kv_paris[entries[index].length-1];
    *last = entry;
    return &last->key;
}

bool map_expand(hashmap* h){
    size_t new_capacity = h->capacity * 2;
    if (new_capacity < h->capacity) {
        return false;  // overflow (capacity would be too big)
    }
    map_slot* new_entries = calloc(new_capacity, sizeof(map_slot));
    if (new_entries == NULL) {
        return false;
    }

   // Iterate entries, move all non-empty ones to new h's entries.
    for (size_t i = 0; i < h->capacity; i++) {
        map_slot entry = h->entries[i];
        for (size_t j = 0; j < entry.length; j++) {
            map_set_entry(new_entries, new_capacity, NULL, entry.kv_paris[j].key,entry.kv_paris[j].value);
        }
    }
    
    free(h->entries);
    h->capacity = new_capacity;
    h->entries = new_entries;
    return true;
}

uint64_t* map_set(hashmap* h, int32_t pos[2], void* value){
    if (h->length >= h->capacity / 2) {
        if (!map_expand(h)) {
            return NULL;
        }
    }
    uint64_t key = pos_to_key(pos);
    uint64_t* result = map_set_entry(h->entries,h->capacity,&h->length,key,value);
    return result;

}

typedef struct{
    bool found;
    void* value;
} result;

result map_get(hashmap* h, int32_t pos[2]){
    uint64_t key = pos_to_key(pos);
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(h->capacity - 1));
    map_slot hashed_slot = h->entries[index];
    if (hashed_slot.length == 0) {
        return (result){false,NULL}; 
    }

    for (size_t i = 0; i < hashed_slot.length; i++) {
        map_entry current_entry = hashed_slot.kv_paris[i];
        if (current_entry.key == key) {
            return (result){true,current_entry.value}; 
        }
    }
    return (result){false,NULL}; 
}
int main(){
    FILE* inputs;
    errno_t err = fopen_s(&inputs , "test.txt", "rb");
    fseek(inputs, 0, SEEK_END);
    size_t read_count = ftell(inputs);
    rewind(inputs);
    char* buffer = malloc(sizeof(char)*(read_count+1));
    fread(buffer,sizeof(char),read_count,inputs);
    buffer[read_count] = '\0';
    char* nline = strchr(buffer, '\n');

    int x_size = nline-buffer;
    int y_size = 0;

    while (nline!=NULL) {
        y_size++;
        nline=strchr(nline+1,'\n');
    }
    int S_index = strchr(buffer,'S')-buffer;
    int E_index = strchr(buffer,'E')-buffer;
    int spos[2] = {0,S_index/x_size};
    spos[0] = S_index - spos[1]*(x_size+1);
    int epos[2] = {0,E_index/y_size};
    epos[0] = E_index - epos[1]*(x_size+1);

    int directions[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
    int current_direction = 0;

    minheap pq = new_minheap();
    mheap_item start_item = {0,{spos[0],spos[1]}};

    minheap_insert(&pq, start_item);
    for (;pq.length > 0;) {
        mheap_item current =  minheap_extract(&pq);
        if (current.pos[0] == epos[0] && current.pos[1] == epos[1]) {
            break; 
        }

        for (int i = 0; i < 4; i++) {
            int* neighbor_vector =  directions[i];
            int neigbor[2] = {current.pos[0]+neighbor_vector[0],current.pos[1]+neighbor_vector[1]};
            
            // if conditions are met:
            mheap_item new_search_position = {1,{neigbor[0],neigbor[1]}};
        }
    }
    free_minheap(&pq);
    free(buffer);
}
