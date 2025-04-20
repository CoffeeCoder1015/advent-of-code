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
    trie* t = malloc(sizeof(trie));
    memset(t->child, 0, sizeof(trie*)*26);
    return t;
}

void trie_insert(trie* t, char* string){
    int n = strlen(string);
    trie* base_node = t;
    for (int i = 0; i < n; i++) {
        int current_index = string[i] - 'a';
        trie** current_node = &base_node->child[current_index];
        if (*current_node == NULL) {
            trie* new_node = make_node();
            new_node->isend = i+1 == n;
            *current_node = new_node ;
            base_node = new_node;
        }else {
            if (i+1 == n) {
                ( *current_node )->isend = true;
            }
            base_node = *current_node;
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
        return false;
    }
}

void free_trie(trie* t){
    int stack_size = 0;
    trie** t_stack = malloc(0);
    for (int i = 0; i < 26; i++) {
        if ( t->child[i] != NULL ) {
            stack_size++;
            t_stack = realloc( t_stack,sizeof(trie*)*stack_size );
            t_stack[stack_size-1] = t->child[i];
        }
    }
    while (stack_size > 0) {
        stack_size--;
        trie* current = t_stack[stack_size];
        for (int i = 0; i < 26; i++) {
            if ( current->child[i] != NULL ) {
                stack_size++;
                t_stack = realloc( t_stack,sizeof(trie*)*stack_size );
                t_stack[stack_size-1] = current->child[i];
            }
        }
        free_trie(current);
    }
}

int main() {
    FILE *inputs;
    fopen_s(&inputs, "q19.txt", "r");
    char raw_substr[4096];
    fgets(raw_substr, 4096, inputs);

    int raw_str_n = strlen(raw_substr);
    raw_substr[raw_str_n-1] = '\0'; // removes trailing \n

    char** raw_split = inplace_split(raw_substr, ", ");
    size_t n_split = (size_t)raw_split[0];
    char** split = &raw_split[1];

    trie t = make_root();
    for (int i = 0; i < n_split; i++) {
        trie_insert(&t, split[i]);
    }
    free(raw_split);

    int amount_possible = 0;

    char buffer[1024];
    fgets(buffer, 1024, inputs); // consumes the empty lines 
    for (;;) {
        char *fg = fgets(buffer, 1024, inputs);
        if (fg == NULL) {
            break;
        }
        int buff_n = strlen(buffer);
        buffer[buff_n-1] = '\0';
        buff_n--;

        // DFS through all possible combinations
        int stack_size = 1;
        int* visited = calloc(1+buff_n,sizeof(int));
        int* index_stack = malloc(sizeof(int));
        index_stack[0] = 0;
        while (stack_size > 0) {
            stack_size--;
            int index = index_stack[stack_size];
            if (index == buff_n) {
                amount_possible++;
                break; 
            }
            // generating "neighbour" by consuming valid prefixes and appending respective indexes
            // as next location to process.
            //
            // If next location (index) is equal to the length of the original string (not suffix string)
            // then a combination has been found!
            char* suffix = &buffer[index];
            int n_suffix = strlen(suffix);
            trie* search_start = &t;
            for (int i = 0; i < n_suffix; i++) {
                bool r = yield_trie_check(&search_start, suffix[i]);
                if (search_start == NULL) {
                    break; 
                }
                if (search_start->isend) {
                    int next_index = index+i+1;
                    if (!visited[next_index]) {
                        stack_size++;
                        index_stack = realloc(index_stack, sizeof(int)*stack_size);
                        index_stack[stack_size-1] = next_index;
                        visited[next_index] = 1;
                    }
                }
            }
        }
        free(index_stack);
        free(visited);
    }

    printf("%d\n",amount_possible);
    free_trie(&t);
}
