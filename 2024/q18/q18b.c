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

Key pos_to_key(void* pos_genric){
    int32_t* pos = pos_genric;
    Key k = new_key(8);

    uint64_t combined = (uint64_t)pos[0] << 32;
    combined |= pos[1];
    for (int i = 0;  i < 8; i++) {
        uint8_t byte_of_data = ( combined >> (i * 8) ) & 255; // grabs 8 bit slices out of the data
        k.bytes[i] = byte_of_data;
    }
    return k;
}

typedef struct{
    int pos[2];
}path_track ;

void free_path_track(map_entry* slot){
    free(slot->value);
}

path_track* new_path_item(int pos[2]){
    path_track* item = malloc(sizeof(path_track));
    item->pos[0] = pos[0];
    item->pos[1] = pos[1];
    return item;
}

// #define TESTING
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
    int size =square_size*(square_size+1);
    char* vmap = malloc(size+1);
    vmap[0] = '\0';
    for (int i = 0; i < square_size*square_size; i++) {
        char cat_obj[] = {gridAtlas[grid[i]],'\0'};
        strcat_s(vmap,size+1,cat_obj);
        if ((i+1)%square_size == 0) {
            strcat_s(vmap,size+1,"\n");
        }
    }
    vmap[size] = '\0';
    printf("%s\n",vmap);
}

uint64_t distance(int pos1[2], int pos2[2]){
    return ( abs( pos1[0]-pos2[0] )<<1 ) + ( abs(pos1[1] - pos2[1])<<1 );
}

result a_star_search(int* memory){
    /* A* setup*/
    minheap pq = new_minheap();
    hashmap* distances = hashmap_new(NULL, pos_to_key);
    hashmap* camefrom = hashmap_new(free_path_track, pos_to_key);

    /* inserting initial values */
    mhitem start_item = {0,{0,0}};
    minheap_insert(&pq, start_item);

    int start_position[2] = {0,0};
    hashmap_set(distances, start_position, 0);
    hashmap_set(camefrom, start_position, new_path_item(start_position));

    /* A* path finding */
    int directions[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};

    int ending_position[2] = {square_size-1,square_size-1};
    for (;pq.heap_length > 0;) {
        mhitem current_pos = minheap_extract(&pq);
        //termination condition
        if (current_pos.pos[0] == ending_position[0] && current_pos.pos[1] == ending_position[1]) {
            break;
        }

        for (int i = 0 ; i < 4; i++) {
            int* direction = directions[i];
            int next_pos[2] = {current_pos.pos[0]+direction[0], current_pos.pos[1]+direction[1]};

            int next_index = next_pos[1]*square_size+next_pos[0];
            bool x_in = 0<= next_pos[0] && next_pos[0] < square_size;
            bool y_in = 0<= next_pos[1] && next_pos[1] < square_size;

            // skip when 'wall' encountered
            if (!(x_in && y_in) || memory[next_index] == 1) { 
                continue; 
            }

            uint64_t current_distance = (uint64_t)hashmap_get(distances, current_pos.pos).value;
            uint64_t predicted_distance = current_distance+1;
            result next_distance = hashmap_get(distances, next_pos);

            if (!next_distance.found || predicted_distance < (uint64_t)next_distance.value ) {
                uint64_t heuristic = distance(next_pos,ending_position);
                mhitem next_frontier = {predicted_distance+heuristic,{next_pos[0],next_pos[1]}};

                minheap_insert(&pq, next_frontier);

                hashmap_set(distances, next_pos, (void*)predicted_distance);

                hashmap_set(camefrom, next_pos, new_path_item(current_pos.pos));

                // visualisation 
                // memory[next_index] = 2;
            }
        }
    }
    
    result r_last_dist = hashmap_get(distances,ending_position);

    /* A* cleanup */
    free_minheap(&pq);
    hashmap_free(distances);
    hashmap_free(camefrom);
    return r_last_dist;
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

    //loading the rest of the coordinates
    int alloc_size = 100;
    int size = 0;
    int (*remaining)[2] = malloc(alloc_size*sizeof(int[2]));
    for (;;) {
        if (line_end == NULL) {
            break; 
        }
        char* sep = strchr(line_start, ',');
        *sep = '\0';
        int x = atoi(line_start);
        *line_end = '\0';
        int y = atoi(sep+1);

        if (size >= alloc_size) {
            alloc_size+=100; 
            int (*new_remaining)[2] = realloc(remaining, sizeof(int[2])*alloc_size);
            remaining = new_remaining;
        }

        size++;
        remaining[size-1][0] = x;
        remaining[size-1][1] = y;

        line_start = line_end+1;
        line_end = strchr(line_end+1, '\n');
    }
    remaining = realloc(remaining, sizeof(int[2])*size);

    int low = 0;
    int high = size-1;

    int mid = ( low+high )/2;
    while (low<=high){
        mid = ( low+high )/2;
        if (low == high) {
            break; 
        }
        for (int i = low; i <= mid; i++) {
            int x = remaining[i][0];
            int y = remaining[i][1];
            memory[y*(square_size)+x] = 1;
        }
        result r = a_star_search(memory);
        if (r.found) {
            low = mid+1; 
        }else {
            high = mid; // not mid-1 because if A* fails the mid point can actually be correct
            // unrolling the memory so it can start again.
            for (int i = low; i <= mid; i++) {
                int x = remaining[i][0];
                int y = remaining[i][1];
                memory[y*(square_size)+x] = 0;
            }
        }
    }
    printf("%d,%d",remaining[mid][0],remaining[mid][1]);
    free(memory);
    free(remaining);
    free(raw_coords);
}
