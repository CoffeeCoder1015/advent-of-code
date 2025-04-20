// DFS over target string
// For neighbour construction
// Trie: Rep. as array; W/ character map; Construction - O(all characters);
// Usage - O(longest substring)
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ptr ptr str str
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
