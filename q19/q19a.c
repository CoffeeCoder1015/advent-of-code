// DFS over target string
// For neighbour construction
// Trie: Rep. as array; W/ character map; Construction - O(all characters);
// Usage - O(longest substring)
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char** inplace_split(char* string, char* sep){
    int sep_n = strlen(sep);
    char* start = string;

    size_t array_size = 0;
    size_t array_capacity = 4;
    char** str_array = malloc(array_capacity*sizeof(char*));
    
    str_array[0] = 0;

    for (;;) {
        char *e_ptr = strpbrk(start, sep);
        if (e_ptr != NULL) {
            *e_ptr = '\0';
        }

        array_size++;
        str_array[0]++;
        if (array_size > array_capacity) {
            array_capacity+=4;
            str_array = realloc(str_array, sizeof(char*)*array_capacity);
        }
        str_array[array_size] = start;

        start = e_ptr+sep_n;
        if (e_ptr == NULL) {
            break;
        }
    }
    if (array_capacity > array_size) {
        str_array = realloc(str_array, sizeof(char*)*array_size);
    }
    return str_array;
}


// store = array of all characters
// map   = contains the pointer directions to make store function as a trie
typedef struct{
    int n;
    int (*map)[26];
} trie;

trie new_trie(){
    trie t = {1,calloc(1,sizeof(int[26]))};
    return t;
}

void insert_trie(trie* t, char* string){
    int trie_index = 0;
    int n = strlen(string);
    for (int i = 0; i < n; i++) {
        int current_index = string[i]-'a';
        int next_index = string[i+1]-'a';

        int mapping = t->map[trie_index][current_index];
        if (mapping == 0) {
            if (next_index < 0) {
                t->map[trie_index][current_index] = -1; //termination character
            }else{
                // create new mapping 
                int new_size = t->n+1;
                int (*new_map)[26] = calloc(new_size , sizeof(int[26]));
                memcpy_s(new_map, new_size*sizeof(int[26]), t->map, t->n*sizeof(int[26]));
                free(t->map);
                t->map = new_map;
                t->map[trie_index][current_index] = t->n; // map to next node
                t->n = new_size;
            }
        }else {
            trie_index = mapping;
        }
    }
}

int check_trie(trie* t,int offset, char character){
    int char_index = character-'a';
    return t->map[offset][char_index];
}


int main() {
    FILE *inputs;
    fopen_s(&inputs, "test.txt", "r");
    char raw_substr[1024];
    fgets(raw_substr, 1024, inputs);

    int raw_str_n = strlen(raw_substr);
    raw_substr[raw_str_n-1] = '\0'; // removes trailing \n

    char** raw_split = inplace_split(raw_substr, ", ");
    size_t n_split = (size_t)raw_split[0];
    char** split = &raw_split[1];
    for (int i = 0; i < n_split; i++) {
        printf("%s\n",split[i]);
    }
    free(raw_split);

    char buffer[1024];
    fgets(buffer, 1024, inputs); // consumes the empty lines 
    for (;;) {
        char *fg = fgets(buffer, 1024, inputs);
        if (fg == NULL) {
            break;
        }
        printf("%s", buffer);
    }
}