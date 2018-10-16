#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <string.h>

#define n 2

static ucontext_t context_main;
static ucontext_t contexts[n];
static int** int_file_array;
static int* int_file_array_size;

#define context_swap(i) {                                 \
    if (((i) + 1) == n) {                                 \
        swapcontext(&contexts[(i)], &contexts[0]);        \
    }                                                     \
    else {                                                \
        swapcontext(&contexts[(i)], &contexts[(i) + 1]);  \
    }                                                     \
}

static void context_function(int left, int right, int ind) { //quick sort
    int i = left, j = right;
    context_swap(ind);
    int tmp;
    context_swap(ind);
    int pivot = int_file_array[ind][(left + right) / 2];
    context_swap(ind);
    while (i <= j) {
        context_swap(ind);
        while (int_file_array[ind][i] < pivot) {
            context_swap(ind);
            i++;
            context_swap(ind);
        }
        context_swap(ind);
        while (int_file_array[ind][j] > pivot) {
            context_swap(ind);
            j--;
            context_swap(ind);
        }
        context_swap(ind);
        if (i <= j) {
            context_swap(ind);
            tmp = int_file_array[ind][i];
            context_swap(ind);
            int_file_array[ind][i] = int_file_array[ind][j];
            context_swap(ind);
            int_file_array[ind][j] = tmp;
            context_swap(ind);
            i++;
            context_swap(ind);
            j--;
            context_swap(ind);
        }
        context_swap(ind);
    };
    context_swap(ind);

    if (left < j) {
        context_swap(ind);
        context_function(left, j, ind);
        context_swap(ind);
    }
    if (i < right) {
        context_swap(ind);
        context_function(i, right, ind);
        context_swap(ind);
    }
    context_swap(ind);
}

void merge_arrays(int* arr1, int* arr2, int n1, int n2, int* arr3)
{
    int i = 0, j = 0, k = 0;

    while (i < n1 && j < n2)
    {
        if (arr1[i] < arr2[j])
            arr3[k++] = arr1[i++];
        else
            arr3[k++] = arr2[j++];
    }
    while (i < n1)
        arr3[k++] = arr1[i++];
    while (j < n2)
        arr3[k++] = arr2[j++];
}

void final_merge() {
    FILE* final_file = fopen("final", "w+");

    int** merged_arrays;
    if (n > 2) {
        merged_arrays = (int**) malloc((n - 1) * sizeof(int *));
        int size = int_file_array_size[0] + int_file_array_size[1];
        merged_arrays[0] = (int*) malloc(size * sizeof(int));
        merge_arrays(int_file_array[0], int_file_array[1], int_file_array_size[0], int_file_array_size[1], merged_arrays[0]);
        for (int i = 2; i < n; i++) {
            size += int_file_array_size[i];
            merged_arrays[i - 1] = (int*) malloc(size * sizeof(int));
            merge_arrays(merged_arrays[i - 2], int_file_array[i], int_file_array_size[i - 2], int_file_array_size[i], merged_arrays[i - 1]);
        }
        for (int i = 0; i < size; i++)
            fprintf(final_file, "%d ", merged_arrays[n - 2][i]);
    }
    else {
        merged_arrays = (int**) malloc(sizeof(int *));
        int size = int_file_array_size[0] + int_file_array_size[1];
        merged_arrays[0] = (int*) malloc(size * sizeof(int));
        merge_arrays(int_file_array[0], int_file_array[1], int_file_array_size[0], int_file_array_size[1], merged_arrays[0]);
        for (int i = 0; i < size; i++) {
            fprintf(final_file, "%d ", merged_arrays[0][i]);
        }
    }

    fclose(final_file);   //cleanup
    if (n > 2) {
        for (int i = 0; i < n - 1; i++) {
            free(merged_arrays[i]);
        }
    }
    else
        free (merged_arrays[0]);
    free(merged_arrays);
}

void parse_file(FILE* f, int i) {
    int t;
    int* tmp;
    int j = 0, size = 1000;
    fscanf(f, "%d", &t);
    int_file_array[i] = (int*) malloc(1000 * sizeof(int));
    int_file_array[i][j] = t;
    while (!feof(f)) {
        fscanf(f, "%d", &t);
        j++;
        if (j > size) {
            tmp = realloc(int_file_array[i], size * 2 * sizeof(int));
            size *= 2;
            int_file_array[i] = tmp;
            tmp = NULL;
        }
        int_file_array[i][j] = t;
    }
    tmp = realloc(int_file_array[i], j * sizeof(int));
    int_file_array[i] = tmp;
    tmp = NULL;
    int_file_array_size[i] = j;
}

int main() {
    printf("Enter %d file names:\n", n);
    FILE** fin = (FILE**) malloc(n * sizeof(FILE*)); //array of all input files
    FILE** fout = (FILE**) malloc(n * sizeof(FILE*)); //array for output files (sorted)
    char s[100];
    for (int i = 0; i < n; i++) {
        scanf("%s", s);
        fin[i] = fopen(s, "r");
        fout[i] = fopen(strcat(s, "_out"), "w+");
        if (fin[i] == NULL || fout[i] == NULL) {
            printf("error opening file");
        }
    }

    char stacks[n][1048576];
    for (int i = 0; i < n; i++) {
        getcontext(&contexts[i]);
        contexts[i].uc_stack.ss_sp = stacks[i];
        contexts[i].uc_stack.ss_size = sizeof(stacks[i]);
        if (i == 0)
            contexts[i].uc_link = &context_main;
        else
            contexts[i].uc_link = &contexts[i - 1];
    }

    int_file_array = (int**) malloc(n * sizeof(int*));        //arrays of all numbers from all files
    int_file_array_size = (int*) malloc(n * sizeof(int));     //all sizes of arrays

    for (int i = 0; i < n; i++) {
        parse_file(fin[i], i);                                //making arrays

        for (int j = 0; j < int_file_array_size[i]; j++) {    //checking
            printf("%d ", int_file_array[i][j]);
        }
        printf("\n");

        makecontext(&contexts[i], context_function, 3, 0, int_file_array_size[i] - 1, i); //associate co-routines with sorting
    }

    swapcontext(&context_main, &contexts[n - 1]);                                         //swapping to context

    for (int i = 0; i < n; i++) {                                                         //checking the sort
        for (int j = 0; j < int_file_array_size[i]; j++) {
            fprintf(fout[i], "%d ", int_file_array[i][j]);
            printf("%d ", int_file_array[i][j]);
    }
    printf("\n");
    }

    final_merge();

    free(int_file_array_size);       //cleanup
    for (int i = 0; i < n; i++) {
        free(int_file_array[i]);
        fclose(fin[i]);
        fclose(fout[i]);
    }
    free(int_file_array);
    free(fin);
    free(fout);
    return 0;
}