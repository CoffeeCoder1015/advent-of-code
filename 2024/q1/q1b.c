#include <corecrt.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
int compare_func(const void *a, const void *b)
{
    int i_a = *((int *)a);
    int i_b = *((int *)b);

    if (i_a < i_b)
        return -1;
    else if (i_a == i_b)
        return 0;
    else
        return 1;
}

int SumArray(int a[], const int numElements)
{
    int k;
    int sum = 0.0;                    /* Edit 3: 0.0 rather than 0 for clarity */
    for (k = 0; k < numElements; ++k) /* no ; here!  --- Edit 3: ++k for speed and good practice */
    {                                 /* ^^^^^^^^^^^ */
        sum += a[k];
    }
    return (sum);
}

char *intArray_str(int *array, size_t n)
{
    size_t alloc_size = sizeof(char) * (n * 2 + 1);
    char *content = (char *)malloc(alloc_size);
    content[0] = 0;
    for (size_t i = 0; i < n - 1; i++)
    {
        char buffer[3];
        sprintf(buffer, "%d,", array[i]);
        strncat_s(content, alloc_size, buffer, 3);
    }
    char buffer[2];
    sprintf(buffer, "%d", array[n - 1]);
    strncat_s(content, alloc_size, buffer, 2);
    return content;
}

typedef struct
{
    int count;
    int id;
} minimap;

int main()
{
    FILE *inputs;
    errno_t err = fopen_s(&inputs, "q1.txt", "r");
    printf("%d", err); // print err

    int num1;
    int num2;
    int A[1000];
    int B[1000];
    int result[1000];
    size_t n = 1000;

    int i = 0;
    while (fscanf_s(inputs, "%d %d", &num1, &num2) == 2)
    {
        A[i] = num1;
        B[i] = num2;
        i++;
    }
    fclose(inputs);

    // int A[] = {3,4,2,1,3,3};
    // int B[] = {4,3,5,3,9,3};
    // int result[6];
    // size_t n = 6;
    size_t size = sizeof(int);
    qsort(A, n, size, compare_func);
    qsort(B, n, size, compare_func);

    // char* printstrA = intArray_str(A,n);
    // char* printstrB =intArray_str(B,n);
    // printf("%s\n%s\n",printstrA,printstrB);

    int count = 0;
    minimap mappedB[1000];
    int unique_count = 0;
    for (size_t i = 0; i < n; i++)
    {
        int current = B[i];
        int next = B[i + 1];
        count++;
        if (current != next)
        {
            minimap m;
            m.count = count;
            m.id = current;
            memcpy(&mappedB[unique_count], &m, sizeof(minimap));
            count = 0;
            unique_count++;
        }
    }

    for (size_t i = 0; i < n; i++)
    {
        int ans = 0;
        int current = A[i];
        for (size_t j = 0; j < unique_count; j++)
        {
            minimap m = mappedB[j];
            if (current < m.id)
            {
                break;
            }
            if (current == m.id)
            {
                ans += current * m.count;
            }
        }
        result[i] = ans;
    }
    printf("%d", SumArray(result, n));
}
