#include <assert.h>
#include <complex.h>
#include <iso646.h>
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

Key vecn_to_key(void* strgeneric){
    int* vec = strgeneric;
    int n = vec[0];
    Key k = new_key(n);
    for (int i = 0; i < n; i++) {
        k.bytes[i] = vec[i];
    }
    return k;
}


// x_end > x_start:
//  - Means moving from left to right (>)
//  if y_end > y_start:
//    - Means moving from top to bottom (v)
//  if y_end < y_start:
//    - Means moving from bottom to top (^)
//
//  Numpad:
//  For this situation, moving (>) and (^) has no posibility of hitting the null square, so no prioity is given between them.
//  When moving (v) and (>) however should prioitise (>) first to move away from x=0 to avoid going into null sqaure.
//  for x+ & y+
//  final: >v
//
//  Dirpad:
//  Using the same analysis, moving up when on (<) on the dirpad would hit the nullsquare.
//  thus, favor (>) over (^)
//  for x+ & y-
//  final: >^
//
//
// x_end < x_start:
//  - Means moving from right to left (<)
//  Same y situation of 
//    - y_end > y_start => (v)
//    - y_end < y_start => (^)
//
//  Numpad:
//  For this case, <^ can cause hitting the nullsquare.
//  ### <v seems like it can, but hats the case when the final target is the nullsquare which is a much bigger problem.
//  For this case, favoring (^) over (<) would allow it to move away from y=3 from the start to avoid the null square.
//  for x- & y-
//  final: ^<
//
//  Dirpad:
//  Same analysis, if starting on (A) or (^) and want to reach (<), moving (<) first would hit the nullsquare. 
//  Therefore the best option is to move (v) first.
//  for x- & y+
//  final: v<
//
//  Combining into a options matrix:
//  * x  +   -
//  y *---------
//    |(N) |(D)
//  + | >v | v<
//    |----+----
//  - | >^ | ^<
//    |(D) |(N)
//
//  Priority of Numpad:
//  >v^< (if its gonna git the n.s)
//  <^v> (reversed, if its not going to)
//  This is reversed in this manner because first moving to (<), the furtheset from the starting point 
//  means less traveling back and forth to the furthest key from (A) for robots down the line.
//
//  Priority for DirPad:
//  >^v<
//  <v^>
//  Notice that it is in the order of distance away from the starting square (A)
//Nunber pad:
// 'A' mapped to index 10, so that the rest of the numbers are a perfect hash
//
//Y=
// 0|7 8 9
// 1|4 5 6
// 2|1 2 3
// 3|  0 A
//  ------
//X=[0 1 2]
//
//
//
//
//                            0     1     2     3     4     5     6     7     8     9     A
int numpad_coords[11][2] = {{1,3},{0,2},{1,2},{2,2},{0,1},{1,1},{2,1},{0,0},{1,0},{2,0},{2,3}};

//Direction pad:
// 'A' mapped to the end so the directons coords are a perfect hash to the previous questions' dir vector list
//
//Y=
// 0|   ^ A
// 1| < v >
//  -------
//X= [0 1 2]
//                           >     v     <     ^     A
int dirpad_coords[5][2] = {{2,1},{1,1},{0,1},{1,0},{2,0}};
char dir_glyph[] = ">v<^A";
int directions[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};

typedef struct{
    int size;
    int* ptr;
} array;

int numpad_map(int x){
    if (x == 'A') {
        return 10; 
    }
    return x - '0';
}

array to_robot_instructions(int mode, int* instructions, int n){
    int base_capacity = 10;
    int base_size = 0;
    int* base_array = malloc(base_capacity*sizeof(int));

    int previous_mapping = 4;  
    if (mode == 0) {
        previous_mapping = 10; 
    }

    for (int i = 0; i < n; i++) {
        int current_mapping = instructions[i];
        int* current = dirpad_coords[current_mapping];
        int* previous = dirpad_coords[previous_mapping];
        if (mode == 0) {
            current_mapping = numpad_map(current_mapping);
            current = numpad_coords[current_mapping];
            previous = numpad_coords[previous_mapping];
        }

        int vec_diff[2] = { current[0]-previous[0], current[1]-previous[1] };
        // printf("M:%d %d,%d -> %d,%d\n",current_mapping,previous[0],previous[1],current[0],current[1]);
        // printf("%c %d,%d\n",instructions[i],vec_diff[0],vec_diff[1]);
        // printf("%c %d,%d\n",dir_glyph[ instructions[i] ],vec_diff[0],vec_diff[1]);

        int x_mag = abs(vec_diff[0]);
        int y_mag = abs(vec_diff[1]);

        int memory_defecit = base_size + x_mag + y_mag + 2  - base_capacity;
        if (memory_defecit > 0) {
            base_capacity += abs(memory_defecit);
            base_array  = realloc(base_array, sizeof(int)*base_capacity);
        }

        int applyx[2] = {current[0],previous[1]};
        int applyy[2] = {previous[0],current[1]};

        bool dirpadhit_nsx = applyx[0] == 0 && applyx[1] == 0;
        bool dirpadhit_nsy = applyy[0] == 0 && applyy[1] == 0;
        bool reverse = dirpadhit_nsx || dirpadhit_nsy; 
        if(mode==0){ 
            bool numpadhit_nsx = applyx[0] == 0 && applyx[1] == 3;
            bool numpadhit_nsy = applyy[0] == 0 && applyy[1] == 3;
            reverse = numpadhit_nsx || numpadhit_nsy; 
        }


        int xneg = vec_diff[0]>>31 | !!vec_diff[0];
        int yneg = vec_diff[1]>>31 | !!vec_diff[1];
        int x_index = 1-xneg;
        int y_index = 2-yneg;

        bool xfirst = xneg==-1 && !reverse || xneg >= 0 && reverse;

        if (xfirst) {
            for (int j = 0; j < x_mag; j++) {
                base_size++; 
                base_array[base_size-1] = 1-xneg;
            }
        }

        for (int j = 0; j < y_mag; j++) {
            base_size++; 
            base_array[base_size-1] = 2-yneg;
        }

        if (!xfirst) {
            for (int j = 0; j < x_mag; j++) {
                base_size++; 
                base_array[base_size-1] = 1-xneg;
            }
        }

        base_size++;
        base_array[base_size-1] = 4;
        previous_mapping = current_mapping;
    }
    return (array){base_size,base_array} ;
}

int main(){
    FILE* input;
    fopen_s(&input, "q21.txt", "rb");

    uint64_t complexity = 0;
    for (;;) {
        char buffer[1024];
        char* end = fgets(buffer, 1024 , input);
        if (end == NULL) {
            break; 
        }
        int n = strlen(buffer);
        buffer[--n] = '\0';

        int ibuffer[1024];
        for (int i = 0; i < n; i++) {
            ibuffer[i]  = buffer[i];
        }

        hashmap* ohashcount = hashmap_new(NULL, vecn_to_key);
        array l1 = to_robot_instructions(0, ibuffer, n);

        int keybuffer_capacity = 10;
        int kb_count = 0;
        int** key_buffer = malloc(sizeof(int*)*keybuffer_capacity);
        int previous_start = -1;
        for (int i = 0; i < l1.size; i++) {
            if (l1.ptr[i] == 4) {
                int key_length = i-previous_start;
                int* key = malloc(sizeof(int)*( key_length+1 ));
                key[0] = key_length+1;
                for (int j = 0; j < key_length ; j++) {
                    key[j+1]  =l1.ptr[previous_start+1+j];
                }
                result r = hashmap_get(ohashcount, key);
                if (r.found) {
                    uint64_t original = (uint64_t)r.value;
                    uint64_t new = 1+original;
                    hashmap_set(ohashcount,key,(void*)new);

                    // key found so clear
                    free(key);
                }else {
                    hashmap_set(ohashcount,key,(void*)1);

                    // new key so apppend
                    kb_count++;
                    if (kb_count > keybuffer_capacity) {
                        keybuffer_capacity += 10; 
                        key_buffer = realloc(key_buffer, sizeof(int*)*keybuffer_capacity);
                    }
                    key_buffer[kb_count-1] = key;
                }
                previous_start = i;
            }
        }

        for (int i = 0; i < 25; i++) {
            hashmap* hashcount = hashmap_new(NULL, vecn_to_key); 
            int newkb_cap = keybuffer_capacity;
            int nkb_count = 0;
            int** newkb = malloc(newkb_cap*sizeof(int*));

            for (int j = 0; j < kb_count; j++) {
                int* key = key_buffer[j];
                result current = hashmap_get(ohashcount,key);
                if (current.found) {
                    array lx = to_robot_instructions(1, &key[1], key[0]-1);

                    int pstart = -1;
                    for (int k = 0; k < lx.size; k++) {
                        if (lx.ptr[k] == 4) {
                            int key_length = k-pstart;
                            int* key = malloc(sizeof(int)*( key_length+1 ));
                            key[0] = key_length+1;

                            for (int l = 0; l < key_length; l++) {
                                key[l+1] = lx.ptr[l+pstart+1];
                            }

                            result r = hashmap_get(hashcount, key);
                            if (r.found) {
                                uint64_t original = (uint64_t)r.value;
                                uint64_t cur_value = (uint64_t)current.value;
                                uint64_t new = cur_value+original;
                                hashmap_set(hashcount,key,(void*)new);

                                //key found
                                free(key);
                            }else {
                                hashmap_set(hashcount,key,current.value);

                                //key not found
                                nkb_count++;
                                if (nkb_count > newkb_cap) {
                                    newkb_cap += 10; 
                                    newkb = realloc(newkb, sizeof(int*)*newkb_cap);
                                }
                                newkb[nkb_count-1] = key;
                            }

                            pstart = k; 
                        } 
                    }

                }
            }

            hashmap_free(ohashcount);
            ohashcount = hashcount;

            for (int j = 0; j < kb_count; j++) {
                free(key_buffer[j]);
            }
            free(key_buffer);
            key_buffer = newkb;
            kb_count = nkb_count;
            keybuffer_capacity = newkb_cap;
        }
        uint64_t char_sum = 0;
        for (int j = 0; j < kb_count; j++) {
            int* cur_arr = key_buffer[j];
            uint64_t kl = cur_arr[0]-1;
            result r = hashmap_get(ohashcount, cur_arr);
            uint64_t mult = (uint64_t)r.value;
            char_sum+=kl*mult;
        }

        hashmap_free(ohashcount);
        for (int j = 0; j < kb_count; j++) {
            free(key_buffer[j]);
        }
        free(key_buffer);

        char* termination = strchr(buffer,'A');
        *termination = '\0';
        uint64_t interger_component = atoi(buffer);
        complexity += interger_component * char_sum;
    }
    fclose(input);
    printf("%llu\n",complexity);
}
