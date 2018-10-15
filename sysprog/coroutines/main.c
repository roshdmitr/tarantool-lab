#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ucontext.h>
#include <string.h>

static ucontext_t context_main, context1, context2;
static int* int_file_array1;      //we can't send to makecontext's function associated with context
static int* int_file_array1_size; //anything but integers so we'll declare the
static int* int_file_array2;      //arrays globally to sort them in contexts
static int* int_file_array2_size;

static void quick_sort_context1(int left, int right) { //just a quick sort algorithm for context1 with switches to context2
    int i = left, j = right;                           //sorts int_file_array1
    swapcontext(&context1, &context2);
    int tmp;
    swapcontext(&context1, &context2);
    int pivot = int_file_array1[(left + right) / 2];
    swapcontext(&context1, &context2);
    /* partition */
    while (i <= j) {
        swapcontext(&context1, &context2);
        while (int_file_array1[i] < pivot) {
            swapcontext(&context1, &context2);
            i++;
            swapcontext(&context1, &context2);
        }
        swapcontext(&context1, &context2);
        while (int_file_array1[j] > pivot) {
            swapcontext(&context1, &context2);
            j--;
            swapcontext(&context1, &context2);
        }
        swapcontext(&context1, &context2);
        if (i <= j) {
            swapcontext(&context1, &context2);
            tmp = int_file_array1[i];
            swapcontext(&context1, &context2);
            int_file_array1[i] = int_file_array1[j];
            swapcontext(&context1, &context2);
            int_file_array1[j] = tmp;
            swapcontext(&context1, &context2);
            i++;
            swapcontext(&context1, &context2);
            j--;
            swapcontext(&context1, &context2);
        }
        swapcontext(&context1, &context2);
    };
    swapcontext(&context1, &context2);

    if (left < j) {
        swapcontext(&context1, &context2);
        quick_sort_context1(left, j);
        swapcontext(&context1, &context2);
    }
    swapcontext(&context1, &context2);
    if (i < right) {
        swapcontext(&context1, &context2);
        quick_sort_context1(i, right);
        swapcontext(&context1, &context2);
    }
    swapcontext(&context1, &context2);
}

static void quick_sort_context2(int left, int right) { //just a quick sort algorithm for context2 with switches to context1
    int i = left, j = right;                           //sorts int_file_array2
    swapcontext(&context2, &context1);
    int tmp;
    swapcontext(&context2, &context1);
    int pivot = int_file_array2[(left + right) / 2];
    swapcontext(&context2, &context1);

    /* partition */
    while (i <= j) {
        swapcontext(&context2, &context1);
        while (int_file_array2[i] < pivot) {
            swapcontext(&context2, &context1);
            i++;
            swapcontext(&context2, &context1);
        }
        swapcontext(&context2, &context1);
        while (int_file_array2[j] > pivot) {
            swapcontext(&context2, &context1);
            j--;
            swapcontext(&context2, &context1);
        }
        swapcontext(&context2, &context1);
        if (i <= j) {
            swapcontext(&context2, &context1);
            tmp = int_file_array2[i];
            swapcontext(&context2, &context1);
            int_file_array2[i] = int_file_array2[j];
            swapcontext(&context2, &context1);
            int_file_array2[j] = tmp;
            swapcontext(&context2, &context1);
            i++;
            swapcontext(&context2, &context1);
            j--;
            swapcontext(&context2, &context1);
        }
        swapcontext(&context2, &context1);
    };
    swapcontext(&context2, &context1);

    if (left < j) {
        swapcontext(&context2, &context1);
        quick_sort_context2(left, j);
        swapcontext(&context2, &context1);
    }
    swapcontext(&context2, &context1);
    if (i < right) {
        swapcontext(&context2, &context1);
        quick_sort_context2(i, right);
        swapcontext(&context2, &context1);
    }
    swapcontext(&context2, &context1);
}

char* read_file(FILE* f) //reading one file to memory
{
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *s = (char*) malloc((size + 1) * sizeof(char));
    fread(s, (size_t)size, 1, f);
    s[size] = '\0';
    printf("%s \n", s);
    return s;
}

int* parse_file(char* s, int *n) //making an array of integers from the read file
{
    int size = 1000;
    int* arr = (int*) malloc(size * sizeof(int));
    int iter = 0;
    int bytes_read, c, length = 0;
    while(sscanf(s, "%d%n", &c, &bytes_read) > 0)
    {
        if (length + 1 >= size) {
            arr = realloc(arr, size * 2 * sizeof(int));
            size *= 2;
        }
        arr[length++] = c;
        s += bytes_read;
        iter++;
    }
    arr = realloc(arr, iter * sizeof(int));
    *n = iter;
    for (int i = 0; i < iter; i++)
        printf("%d ", arr[i]);
    printf("\n");
    return arr;
}

int main() {
    printf("Enter the number of files you want to read:\n");
    int n;
    scanf("%d", &n);
    printf("Enter file names:\n");
    FILE** fin = (FILE**) malloc(n * sizeof(FILE*)); //array of all input files
    FILE** fout = (FILE**) malloc(n * sizeof(FILE*)); //array for output files (sorted)
    char s[100];
    for (int i = 0; i < n; i++) {
        scanf("%s", s);
        fin[i] = fopen(s, "r");
        fout[i] = fopen(strcat(s, "_out"), "w+t");
        if (fin[i] == NULL || fout[i] == NULL) {
            printf("error opening file");
        }
    }

    static char context1_stack[63484];
    static char context2_stack[63484];

    getcontext(&context1);
    context1.uc_stack.ss_sp = context1_stack;
    context1.uc_stack.ss_size = sizeof(context1_stack);
    context1.uc_link = &context_main;

    getcontext(&context2);
    context2.uc_stack.ss_sp = context2_stack;
    context2.uc_stack.ss_size = sizeof(context2_stack);
    context2.uc_link = &context1;

    char* string_file_array1;
    int_file_array1_size = (int*) malloc(sizeof(int)); //it was declared globally, so let's allocate some memory for it
    char* string_file_array2;
    int_file_array2_size = (int*) malloc(sizeof(int)); //same
    for (int i = 0; i < n / 2; i++) {
        string_file_array1 = read_file(fin[i]); //read the first file
        int_file_array1 = parse_file(string_file_array1, int_file_array1_size); //make an array from the first file
        string_file_array2 = read_file(fin[i + 1]); //read the second file
        int_file_array2 = parse_file(string_file_array2, int_file_array2_size); //make an array from the second file

        makecontext(&context1, quick_sort_context1, 2, 0, *int_file_array1_size - 1); //associate context1 with quick_sort1
        makecontext(&context2, quick_sort_context2, 2, 0 , *int_file_array2_size - 1); //associate context2 with quick_sort2

        swapcontext(&context_main, &context2);
        for (int j = 0; j < *int_file_array1_size; j++) //print first sorted array
            fprintf(fout[i], "%d ", int_file_array1[j]);
        for (int j = 0; j < *int_file_array2_size; j++) //print second sorted array
            fprintf(fout[i + 1], "%d ", int_file_array2[j]);
        free(string_file_array1); //some cleanup because files don't fit into memory all together
        free(int_file_array1);
        free(string_file_array2);
        free(int_file_array2);
    }
    free(int_file_array1_size);
    free(int_file_array2_size);

    for (int i = 0; i < n; i++) {
        fclose(fin[i]);
        fclose(fout[i]);
    }
    free(fin);
    free(fout);
    return 0;
}