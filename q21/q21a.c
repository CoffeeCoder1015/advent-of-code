#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Nunber pad:
// 'A' mapped to index 10, so that the rest of the numbers are a perfect hash
//
//Y=
// 3|7 8 9
// 2|4 5 6
// 1|1 2 3
// 0|  0 A
//  ------
//X=[2 1 0]
//
//
//
//
//                            0     1     2     3     4     5     6     7     8     9     A
int numpad_coords[11][2] = {{1,0},{2,1},{1,1},{0,1},{2,2},{1,2},{0,2},{2,3},{1,3},{0,3},{0,0}};

//Direction pad:
// 'A' mapped to the end so the directons coords are a perfect hash to the previous questions' dir vector list
//
//Y=
// 0|   ^ A
// 1| < v >
//  -------
//X= [2 1 0]
//                           >     v     <     ^     A
int dirpad_coords[5][2] = {{0,1},{1,1},{2,1},{1,0},{0,0}};
char dir_glyph[] = ">v<^";
int directions[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};

int main(){
    FILE* input;
    fopen_s(&input, "test.txt", "rb");

    for (;;) {
        char buffer[1024];
        char* end = fgets(buffer, 1024 , input);
        if (end == NULL) {
            break; 
        }

        printf("%s",buffer);
        char* termination = strchr(buffer,'A');
        *termination = '\0';
        int interger_component = atoi(buffer);
        printf("%d\n",interger_component);
    }
}
