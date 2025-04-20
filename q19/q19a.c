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
typedef struct trie trie;

struct trie{
    bool isend;
    trie* child[26];
};

trie make_root(){
    return (trie){false};
}

trie* make_node(){
    printf("t made!\n");
    trie* t = malloc(sizeof(trie));
    memset(t->child, 0, sizeof(trie*)*26);
    return t;
}

void trie_insert(trie* t, char* string){
    int n = strlen(string);
    trie* current_node = t;
    for (int i = 0; i < n; i++) {
        int current_index = string[i] - 'a';
        trie** next_node = &current_node->child[current_index];
        if (*next_node == NULL) {
            trie* new_node = make_node();
            new_node->isend = i+1 == n;
            *next_node = new_node ;
            current_node = new_node;
        }else {
            current_node = *next_node;
        }
    }
}

bool yield_trie_check(trie** t_yield, char check){
    trie* t = *t_yield;
    int check_index = check-'a';
    trie* next_node = t->child[check_index];
    *t_yield = next_node;
    if (next_node != NULL) {
        return true;  
    }else {
        return next_node->isend;
    }
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