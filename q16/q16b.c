#include <corecrt.h>
#include <math.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// referncing https://web.stanford.edu/class/archive/cs/cs161/cs161.1168/lecture4.pdf
// implementing heap insertion, heap deletion 
// heap inc/dec key is also not needed
// heap extractkey is just a compound action of get array[0] and dropping array[0]
// and reheapiydying
//

typedef struct{
    uint64_t potential;
    int dir_index;
    int pos[2];
} mheap_item;

typedef struct{
    mheap_item* heap;
    int length;
} minheap;

void check_heap(minheap* mh){
    int length = mh->length;
    if (length == 0) {
        printf("Nothing is here\n");
        return;
    }
    bool good = true;
    for (int i = 0;i < length; i++) {
        mheap_item current_node = mh->heap[i];
        uint64_t current_potential = current_node.potential;
        int child1_index = 2*i+1;
        int child2_index = child1_index+1;
        if (child1_index < length) {
            uint64_t child_potential = mh->heap[child1_index].potential;
            if (current_potential > child_potential) {
                good = false ;
                printf("Failed at node [%d]: Parent=%llu Child1=%llu\n",i,current_potential,child_potential);
            } 
        }
        if (child2_index < length) {
            uint64_t child_potential = mh->heap[child2_index].potential;
            if (current_potential > child_potential) {
                good = false ;
                printf("Failed at node [%d]: Parent=%llu Child2=%llu\n",i,current_potential,child_potential);
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
        mheap_item parent = mh->heap[(i-1)/2];
        if (parent.potential > current.potential) {
            minheap_swap_index(mh,i , (i-1)/2);
        }else {
            break;
        }
    }
    // check_heap(mh);
}

void minheap_heapify(minheap* mh, int index){
    int length = mh->length;
    if (length <= 1) {
        return; 
    }
    int start_index = index;
    for (;;) {
        int left_index = 2*start_index+1;
        int right_index = 2*start_index+2;

        int smallest_index = start_index;
        bool has_space_left = left_index < length;
        bool has_space_right = right_index < length;
        if (has_space_left && mh->heap[left_index].potential < mh->heap[start_index].potential) {
            smallest_index = left_index; 
        }
        if (has_space_right && mh->heap[right_index].potential < mh->heap[smallest_index].potential) {
            smallest_index = right_index; 
        }
        if (smallest_index != start_index) {
            minheap_swap_index(mh, start_index, smallest_index);
            start_index = smallest_index;
        }else {
            break;
        }
    }
}

void minheap_delete(minheap* mh){
    int length = mh->length;
    if (length == 0) {
        return; 
    }
    mheap_item last = mh->heap[length-1];
    mh->heap[0] = last;
    mh->length--;
    length--;
    minheap_heapify(mh, 0);
}

mheap_item minheap_extract(minheap* mh){
    mheap_item item = mh->heap[0];
    minheap_delete(mh);
    return item;
}

typedef struct{
    int len;
    int8_t* id;
} Key;

typedef struct{
    Key key;
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
            for (size_t j = 0; j < h->entries[i].length; j++) {
                Key k = h->entries[i].kv_paris[j].key;
                free(k.id);
            }
            free(h->entries[i].kv_paris);
        }
    }
    free(h->entries);
    free(h);
}

bool compare_key(Key key1, Key key2){
    if (key1.len != key2.len) {
        return false;
    }
    for (int i = 0; i < key1.len; i++) {
        if (key1.id[i]!=key2.id[i]) {
            return false;
        }
    }
    return true;
}

Key new_key(int len){
    Key k = { len, malloc(sizeof(int8_t)*len) };
    return k;
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
static uint64_t hash_key(Key key) {
    uint64_t hash = FNV_OFFSET;
    for (int i= 0; i < key.len; i++) {
        uint8_t byte_of_data = key.id[i];
        hash *= FNV_PRIME;
        hash ^= byte_of_data;
    }
    return hash;
}

Key pos_to_key(int dir, int32_t pos[2]){
    Key k = new_key(9);

    uint64_t combined = (uint64_t)pos[0] << 32;
    combined |= pos[1];
    for (int i = 0;  i < 8; i++) {
        uint8_t byte_of_data = ( combined >> (i * 8) ) & 255;
        k.id[i] = byte_of_data;
    }
    k.id[8] = dir;
    return k;
}

Key* map_set_entry(map_slot* entries, size_t capacity, size_t* plength, Key key, void* value) {
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

    for (size_t i = 0; i < entries[index].length; i++) {
        map_entry* current_entry = &entries[index].kv_paris[i];
        if (compare_key(current_entry->key, key)) {
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

Key* map_set(hashmap* h, Key key, void* value){
    if (h->length >= h->capacity / 2) {
        if (!map_expand(h)) {
            return NULL;
        }
    }

    Key* result = map_set_entry(h->entries,h->capacity,&h->length,key,value);
    return result;

}

typedef struct{
    bool found;
    void* value;
} result;

result map_get(hashmap* h,Key key){
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(h->capacity - 1));
    map_slot hashed_slot = h->entries[index];
    if (hashed_slot.length == 0) {
        free(key.id);
        return (result){false,NULL}; 
    }

    for (size_t i = 0; i < hashed_slot.length; i++) {
        map_entry current_entry = hashed_slot.kv_paris[i];
        if (compare_key(current_entry.key, key)) {
            free(key.id);
            return (result){true,current_entry.value}; 
        }
    }
    return (result){false,NULL}; 
}


typedef struct{
    bool isStart;
    int dir;
    int pos[2];
}path_track ;

path_track new_path_item(bool isStart,int dir,int pos[2]){
    path_track item = {};
    item.isStart = isStart;
    item.dir = dir;
    item.pos[0] = pos[0];
    item.pos[1] = pos[1];
    return item;
}

bool cmp_path(path_track p1, path_track p2){
    return  p1.isStart == p2.isStart && p1.dir == p2.dir && p1.pos[0] == p2.pos[0] &&p1.pos[1] == p2.pos[1];    
}

typedef struct{
    int length;
    path_track* paths;
} multiPath;

multiPath* new_mp(){
    multiPath* mp = malloc(sizeof(multiPath));
    mp->length = 1;
    mp->paths = malloc(sizeof(path_track));
    return mp;
}

void free_mp(map_entry* slot){
    multiPath v = *(multiPath*)slot->value;
    free(v.paths);
    free(slot->value);
}

void append_path(multiPath* mp,path_track path){
    mp->length++;
    mp->paths = realloc(mp->paths,  sizeof(path_track)*mp->length);
    mp->paths[mp->length-1] = path;
}

void better_path(multiPath* mp, path_track path){
    mp->length = 1;
    mp->paths = realloc(mp->paths,  sizeof(path_track)*mp->length);
    mp->paths[mp->length-1] = path;
}

typedef struct{
    uint64_t distance;
    int dir_of_distance;
} distance_dir;

int modulo(int x, int y){
    //why does the C modulo operator seem to only work on positive numbers, I will never know. . . .

    double d_x = x;
    double d_y = y;
    double div = floor(d_x/d_y);
    double mod = d_x-div*d_y;
    return mod;
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

    char* dir_glyph = ">v<^";
    int directions[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};

    minheap pq = new_minheap();
    mheap_item start_item = {0,0,{spos[0],spos[1]}};
    minheap_insert(&pq, start_item);

    hashmap* distances = new_hashmap(NULL);
    map_set(distances, pos_to_key(0, spos), 0);

    hashmap* came_from = new_hashmap(free_mp);
    multiPath* start = new_mp();
    append_path(start, new_path_item(true,0, spos));
    map_set(came_from, pos_to_key(0, spos), start);

    for (;pq.length > 0;) {
        mheap_item current =  minheap_extract(&pq);
        if (current.pos[0] == epos[0] && current.pos[1] == epos[1]) {
            result curr_neighbor_r = map_get(distances, pos_to_key(current.dir_index, current.pos));
            printf("p:%llu ( %d,%d ) %d\n",(uint64_t)curr_neighbor_r.value,current.pos[0],current.pos[1],pq.length);
        }

        //old direction
        int current_dir = current.dir_index;
        int possible_dir_index[3] = {current_dir,current_dir+1,current_dir-1};
        for (int i = 0; i < 3; i++) {
            int dir_index = modulo(possible_dir_index[i], 4);
            // int dir_index = i;
            int *neighbor_vector =  directions[dir_index];
            int neighbor[2] = {current.pos[0]+neighbor_vector[0],current.pos[1]+neighbor_vector[1]};
            int neighbor_index = neighbor[1]*(x_size+1)+neighbor[0];
            if (buffer[neighbor_index] == '#') {
                continue; 
            }
            //new direction (just i)
            // heuristic
            int h = ( current_dir != dir_index )*1000;

            uint64_t neighbor_distance = 1; // just grid squares for now
            uint64_t new_distance = (uint64_t)map_get(distances, pos_to_key(current_dir, current.pos)).value+neighbor_distance+h;
            result curr_neighbor_r = map_get(distances, pos_to_key(dir_index, neighbor));
            uint64_t current_neighbor_distance = (uint64_t)curr_neighbor_r.value ;
            if (curr_neighbor_r.found == false|| new_distance <= current_neighbor_distance ) {
                mheap_item new_search_position = {neighbor_distance,dir_index,{neighbor[0],neighbor[1]}};
                minheap_insert(&pq, new_search_position);
                map_set(distances, pos_to_key(dir_index, neighbor), (void*)new_distance);
                // buffer[neighbor_index] = dir_glyph[dir_index];
                if (curr_neighbor_r.found == false) {
                    multiPath* new = new_mp();
                    append_path(new, new_path_item(false, current_dir,current.pos));
                    map_set(came_from, pos_to_key(dir_index, neighbor), new);
                }else if (new_distance == current_neighbor_distance) {
                    multiPath* existing_path = map_get(came_from, pos_to_key(dir_index, neighbor)).value;
                    path_track new_path = new_path_item(false, current_dir, current.pos);
                    if (!cmp_path( new_path, existing_path->paths[existing_path->length - 1])) {
                      append_path(existing_path, new_path);
                    }
                }else if (new_distance < current_neighbor_distance) {
                    multiPath* existing_path = map_get(came_from, pos_to_key(dir_index, neighbor)).value;
                    better_path(existing_path,new_path_item(false, current_dir,current.pos));
                }
                // printf("n(%llu): %d %d c: %d %d\n",new_distance,neighbor[0],neighbor[1],current.pos[0],current.pos[1]);
            }
        }
    }
    free_minheap(&pq);

    bool set = false;
    uint64_t target_dist;
    for (int dir = 0 ; dir < 4; dir++) {
        result end = map_get(distances,pos_to_key(dir, epos));
        if (end.found == true) {
            uint64_t d = (uint64_t)end.value;
            if (d <= target_dist || !set ) {
                target_dist = d;
                set = true;
            }
        }
    }
    printf("%llu\n", target_dist);
/*     while (!current_node.isStart) {
        int current_index = current_node.pos[1] * (x_size + 1) + current_node.pos[0];
        path_track prev_node = *(path_track *)map_get(came_from, pos_to_key(current_node.dir, current_node.pos)).value;
        int x_diff = current_node.pos[0] - prev_node.pos[0];
        int y_diff = current_node.pos[1] - prev_node.pos[1];
        int sum = x_diff + y_diff;
        int glyph_index;
        if (sum == 1) {
            glyph_index = y_diff;
        } else {
            if (x_diff == -1) {
                glyph_index = 2;
            } else {
                glyph_index = 3;
            }
        }
        buffer[current_index] = dir_glyph[glyph_index];
        current_node = prev_node;
    } */
    printf("%s\n", buffer);
    free(buffer);
    free_hashmap(came_from);
    free_hashmap(distances);
}
