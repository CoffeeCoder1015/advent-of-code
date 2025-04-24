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
//
//  Priority for DirPad:
//  >^v<
//  <v^>
//  Notice that it is in the order of distance away from the starting square (A)

//nunber pad:
// 'A' mapped to index 10, so that the rest of the numbers are a perfect hash
//
//Y=
// 0|7 8 9
// 1|4 5 6
// 2|1 2 3
// 3|  0 A
//  ------
//X=[0 1 2]
//                            0     1     2     3     4     5     6     7     8     9     A
int numpad_coords[11][2] = {{1,3},{0,2},{1,2},{2,2},{0,1},{1,1},{2,1},{0,0},{1,0},{2,0},{2,3}};

//direction pad:
// 'a' mapped to the end so the directons coords are a perfect hash to the previous questions' dir vector list
//
//y=
// 0|   ^ A
// 1| < v >
//  -------
//x= [0 1 2]
//                           >     v     <     ^     A
int dirpad_coords[5][2] = {{2,1},{1,1},{0,1},{1,0},{2,0}};
char dir_glyph[] = ">v<^A";
int directions[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};


int numpad_map(int x){
    if (x == 'A') {
        return 10; 
    }
    return x - '0';
}

bool vec_eq(int v1[2], int v2[2]){
    return v1[0] == v2[0] && v1[1] == v2[1];
}

char* apply_mapping(hashmap* map, char* input){
    int n = strlen(input);

    int outcap = n;
    int outsize = 0;
    char* output = malloc(outcap);
    output[0] = '\0';

    char previous = 'A';
    for (int i = 0; i < n; i++) {
        char current = input[i];
        char query[] = {previous,current,'\0'};
        result r = hashmap_get(map,query);
        // printf("%s %s\n",query,r.value);

        if (r.found) {
            char* val = r.value;
            int val_size = strlen(val);
            if (val_size + 1 + outsize >= outcap) {
                outcap += val_size+2; 
                output = realloc(output,outcap);
            }
            assert(val_size+1+outsize <= outcap);
            strcat_s(output, outcap, val);
            outsize+=val_size+1;
        }

        previous = current;
    }
    return output;
}

int main(){
    FILE* input;
    fopen_s(&input, "q21.txt", "rb");

    hashmap* mappings = hashmap_new(NULL, str_to_key);
    hashmap_set(mappings, "A^", "<A");
    hashmap_set(mappings, "A>", "vA");
    hashmap_set(mappings, "Av", "v<A");
    hashmap_set(mappings, "A<", "v<<A");
    hashmap_set(mappings, "AA", "A");

    hashmap_set(mappings, "^A", ">A");
    hashmap_set(mappings, "^v", "vA");
    hashmap_set(mappings, "^>", "v>A");
    hashmap_set(mappings, "^<", "v<A");
    hashmap_set(mappings, "^^", "A");

    hashmap_set(mappings, ">A", "^A");
    hashmap_set(mappings, ">v", "<A");
    hashmap_set(mappings, ">^", "<^A");
    hashmap_set(mappings, "><", "<<A");
    hashmap_set(mappings, ">>", "A");

    hashmap_set(mappings, "v>", ">A");
    hashmap_set(mappings, "v^", "^A");
    hashmap_set(mappings, "v<", "<A");
    hashmap_set(mappings, "vA", ">^A");
    hashmap_set(mappings, "vv", "A");

    hashmap_set(mappings, "<v", ">A");
    hashmap_set(mappings, "<^", ">^A");
    hashmap_set(mappings, "<>", ">>A");
    hashmap_set(mappings, "<A", ">>^A");
    hashmap_set(mappings, "<<", "A");

    for (int i = 0; i < 11; i++) {
        int* vec_start = numpad_coords[i];
        for (int j = 0; j < 11; j++) {
            int* vec_end = numpad_coords[j];
            int vec_diff[2] = {vec_end[0]-vec_start[0],vec_end[1]-vec_start[1]};
            int xmag = abs(vec_diff[0]);
            int ymag = abs(vec_diff[1]);
            char buffer[1024];
            buffer[0] = '\0';
            int x_index = 1-(vec_diff[0]>>31 | !!vec_diff[0]);
            int y_index = 2-(vec_diff[1]>>31 | !!vec_diff[1]);
            int n = xmag+ymag;
            int applyx[2] = {vec_end[0],vec_start[1]};
            int applyy[2] = {vec_start[0],vec_end[1]};
            bool norm = !(applyy[0] == 0 && applyy[1] == 3) && !(applyx[0] == 0 && applyx[1] == 3);
            if (norm) {
                if (vec_diff[0] > 0) {
                    assert(y_index == 1 || y_index == 3 || ymag == 0);
                    for (int j = 0; j < ymag ; j++) {
                        buffer[j] = dir_glyph[y_index];
                    }
                    for (int j = ymag; j < n ; j++) {
                        buffer[j] = '>';
                    }
                }else {
                    // printf("%d vec_diff[1]:%d\n",y_index,vec_diff[1]);
                    for (int j = 0; j < xmag; j++) {
                        buffer[j] = '<';
                    }
                    assert(y_index == 1 || y_index == 3 || ymag == 0);
                    for (int j = xmag; j < n ; j++) {
                        buffer[j] = dir_glyph[y_index];
                    }
                    // printf("%d,%d %d,%d\n",vec_start[0],vec_start[1],vec_end[0],vec_end[1]);
                }
            }else {
                if (vec_diff[0] > 0) {
                    for (int j = 0; j < xmag ; j++) {
                        buffer[j] = '>';
                    }
                    assert(y_index == 1 || y_index == 3 || ymag == 0);
                    for (int j = xmag; j < n ; j++) {
                        buffer[j] = dir_glyph[y_index];
                    }
                }else {
                    // printf("%d vec_diff[1]:%d\n",y_index,vec_diff[1]);
                    assert(y_index == 1 || y_index == 3 || ymag == 0);
                    for (int j = 0; j < ymag ; j++) {
                        buffer[j] = dir_glyph[y_index];
                    }
                    for (int j = ymag; j < n; j++) {
                        buffer[j] = '<';
                    }
                    // printf("%d,%d %d,%d\n",vec_start[0],vec_start[1],vec_end[0],vec_end[1]);
                }
            }
            buffer[n] = 'A';
            buffer[n+1] = '\0';

            char first = i+'0';
            char second = j+'0';
            if (i == 10) {
                first = 'A';
            }
            if (j == 10) {
                second = 'A';
            }
            char key[] = {first,second,'\0'};

            hashmap_set(mappings, key, _strdup(buffer));

        }
    }
    // for (int i = 0; i < 11; i++) {
    //     for (int j = 0; j < 11; j++) {
    //         char first = i+'0';
    //         char second = j+'0';
    //         if (i == 10) {
    //             first = 'A';
    //         }
    //         if (j == 10) {
    //             second = 'A';
    //         }
    //         char key[] = {first,second,'\0'};
    //
    //         //checking
    //         result r = hashmap_get(mappings,key);
    //         printf("%s %s\n",key,r.value);
    //     }
    // }


    int complexity = 0;
    for (;;) {
        char buffer[1024];
        char* end = fgets(buffer, 1024 , input);
        if (end == NULL) {
            break; 
        }
        int n = strlen(buffer);
        buffer[--n] = '\0';

        printf("%s\n",buffer);
        char* l1 = apply_mapping(mappings, buffer);
        char* l2 = apply_mapping(mappings, l1);
        char* l3 = apply_mapping(mappings, l2);
        printf("%s %d\n",l1,strlen(l1));
        printf("%s %d\n",l2,strlen(l2));
        printf("%s %d\n",l3,strlen(l3));
        free(l1);
        free(l2);
        free(l3);
        char* termination = strchr(buffer,'A');
        *termination = '\0';
        int interger_component = atoi(buffer);
        complexity += strlen(l3)*interger_component;
    }
    printf("%d\n",complexity);
    hashmap_free(mappings);
}
