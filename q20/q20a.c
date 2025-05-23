#include <math.h>
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

int modulo(int x, int y){
    //why does the C modulo operator seem to only work on positive numbers, I will never know. . . .
    double d_x = x;
    double d_y = y;
    double div = floor(d_x/d_y);
    double mod = d_x-div*d_y;
    return mod;
}

int main(){
    FILE* input;
    fopen_s(&input, "q20.txt", "rb");

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

    char* dir_glyph = ">v<^";
    int directions[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
    int current_dir = 0;
    int current_pos[2] = {start_pos[0],start_pos[1]};

    int capacity = 20;
    int size = 0;
    int (*path_array)[2] = malloc(capacity*sizeof(int[2]));

    hashmap* distances = hashmap_new(NULL, pos_to_key);

    for (;;) {
        // recording all the distances for fast query
        hashmap_set(distances, current_pos, (void*)(uint64_t)size);

        // constructing an ordered array of path around the track 
        path_array[size][0] = current_pos[0];
        path_array[size][1] = current_pos[1];
        size++;
        if (size == capacity) {
            capacity += 20; 
            path_array = realloc(path_array, sizeof(int[2])*capacity);
        }

        // actual path finding through the racetrack
        if (current_pos[0] == end_pos[0] && current_pos[1] == end_pos[1]) {
            break; 
        }

        int* direction = directions[current_dir];
        int next_pos[2] = {current_pos[0]+direction[0],current_pos[1]+direction[1]};
        int next_index = next_pos[1]*(x_size+1)+next_pos[0];

        //change direction when original direction hits a wall
        if (map[next_index] == '#') {
            int possible_dir_index[2] = {current_dir+1,current_dir-1};
            for (int i = 0; i < 2; i++) {
                int dir_index = modulo(possible_dir_index[i], 4);
                int* new_direction = directions[dir_index];
                next_pos[0] = current_pos[0]+new_direction[0];
                next_pos[1] = current_pos[1]+new_direction[1];
                next_index = next_pos[1]*(x_size+1)+next_pos[0];
                if (map[next_index] == '.') {
                    current_dir = dir_index;
                    break;
                }
            }
        }

        // setting next point to process
        current_pos[0] = next_pos[0];
        current_pos[1] = next_pos[1];
    }

    // resizing oversized path array
    if (capacity > size) {
        path_array = realloc(path_array, size*sizeof(int[2]));
    }

    // calculating skip times
    //
    // Covolution pattern:
    //
    //     X
    //     #
    //   X#O#X
    //     #
    //     X
    //
    // O = current positon
    // X = skipable positions to check
    // # = where walls should be
    
    int skips_at_least_100 = 0;

    for (int i = 0; i < size; i++) {
        int* current_pos = path_array[i];
        int current_index = current_pos[1]*(x_size+1)+current_pos[0];
        for (int d_idx = 0; d_idx < 4; d_idx++) {
            int* direction = directions[d_idx];
            int wall_check[2] = {current_pos[0]+direction[0],current_pos[1]+direction[1]};
            int target_position[2] = {current_pos[0]+2*direction[0],current_pos[1]+2*direction[1]};

            bool x_in = 0 <= target_position[0] && target_position[0] < x_size;
            bool y_in = 0 <= target_position[1] && target_position[1] < y_size;
            if (!( x_in && y_in )) {
                continue; 
            }

            int target_index = target_position[1]*(x_size+1)+target_position[0];
            int wall_index = wall_check[1]*(x_size+1)+wall_check[0];
            if( map[wall_index] == '#' && map[target_index] == '.' || map[target_index] == 'E'){ // valid skip
                // int dist_to_here = i;
                int dist_to_target = (uint64_t)hashmap_get(distances, target_position).value;
                int dist_from_here_to_target = dist_to_target-i; // dist to `here` is just i

                // dist between `target` and `here` after skip is 2
                int dist_skipped = dist_from_here_to_target-2;
                if (dist_skipped >= 100) {
                    skips_at_least_100++;
                }
            }
        }
    }

    printf("%d\n",skips_at_least_100);
    hashmap_free(distances);
    free(path_array);
    free(map);
}
