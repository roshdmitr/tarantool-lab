#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ucontext.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>

#define stack_size 131072

static ucontext_t context_main;
static ucontext_t* contexts;
static bool* is_finished;
static int** int_file_array;
static int* int_file_array_size;
static int n;

#define handle_error(msg) {                                               \
   perror(msg);                                                           \
   exit(EXIT_FAILURE);                                                    \
}

#define check_malloc(ptr) {                                               \
    if ((ptr) == NULL) {                                                  \
        handle_error("malloc()");                                         \
    }                                                                     \
}

#define check_realloc(ptr) {                                              \
    if ((ptr) == NULL) {                                                  \
        handle_error("realloc()");                                        \
    }                                                                     \
}

#define check_fopen(ptr) {                                                \
    if ((ptr) == NULL) {                                                  \
        handle_error("fopen()");                                          \
    }                                                                     \
}

#define check_fclose(return_value) {                                      \
    if ((return_value) == EOF) {                                          \
        handle_error("fclose()");                                         \
    }                                                                     \
}

#define context_swap(i, begin, end, sum) {                                \
    gettimeofday(&(end), NULL);                                           \
    (sum).tv_sec += (end).tv_sec - (begin).tv_sec;                        \
    (sum).tv_usec += (end).tv_usec - (begin).tv_usec;                     \
    if (((i) + 1) == n) {                                                 \
        if (swapcontext(&contexts[(i)], &contexts[0]) == -1) {            \
            handle_error("swapcontext()");                                \
        }                                                                 \
        gettimeofday(&(begin), NULL);                                     \
    }                                                                     \
    else {                                                                \
        if (swapcontext(&contexts[(i)], &contexts[(i) + 1]) == -1) {      \
            handle_error("swapcontext()");                                \
        }                                                                 \
        gettimeofday(&(begin), NULL);                                     \
    }                                                                     \
}                                                                         \

static void context_function(int l, int h, int ind)            //iterative quick sort
{
    struct timeval begin, end, sum;
    gettimeofday(&begin, NULL);
    sum.tv_sec = 0;
    sum.tv_usec = 0;
    context_swap(ind, begin, end, sum);
    int* stack = (int*) malloc((h - l + 1) * sizeof(int));
    context_swap(ind, begin, end, sum);
    check_malloc(stack);
    context_swap(ind, begin, end, sum);

    int top = -1;
    context_swap(ind, begin, end, sum);

    stack[ ++top ] = l;
    context_swap(ind, begin, end, sum);
    stack[ ++top ] = h;
    context_swap(ind, begin, end, sum);

    while ( top >= 0 )
    {
        context_swap(ind, begin, end, sum);
        h = stack[ top-- ];
        context_swap(ind, begin, end, sum);
        l = stack[ top-- ];
        context_swap(ind, begin, end, sum);

        int p;
        context_swap(ind, begin, end, sum);
        int x = int_file_array[ind][h];
        context_swap(ind, begin, end, sum);
        int i = (l - 1);
        context_swap(ind, begin, end, sum);

        for (int j = l; j <= h - 1; j++)
        {
            context_swap(ind, begin, end, sum);
            if (int_file_array[ind][j] <= x)
            {
                context_swap(ind, begin, end, sum);
                i++;
                context_swap(ind, begin, end, sum);
                int t = int_file_array[ind][i];
                context_swap(ind, begin, end, sum);
                int_file_array[ind][i] = int_file_array[ind][j];
                context_swap(ind, begin, end, sum);
                int_file_array[ind][j] = t;
                context_swap(ind, begin, end, sum);
            }
            context_swap(ind, begin, end, sum);
        }
        int t = int_file_array[ind][i + 1];
        context_swap(ind, begin, end, sum);
        int_file_array[ind][i + 1] = int_file_array[ind][h];
        context_swap(ind, begin, end, sum);
        int_file_array[ind][h] = t;
        context_swap(ind, begin, end, sum);
        p = i + 1;
        context_swap(ind, begin, end, sum);

        if ( p - 1 > l )
        {
            context_swap(ind, begin, end, sum);
            stack[ ++top ] = l;
            context_swap(ind, begin, end, sum);
            stack[ ++top ] = p - 1;
            context_swap(ind, begin, end, sum);
        }
        context_swap(ind, begin, end, sum);

        if ( p + 1 < h )
        {
            context_swap(ind, begin, end, sum);
            stack[ ++top ] = p + 1;
            context_swap(ind, begin, end, sum);
            stack[ ++top ] = h;
            context_swap(ind, begin, end, sum);
        }
        context_swap(ind, begin, end, sum);
    }
    context_swap(ind, begin, end, sum);
    is_finished[ind] = true;
    free(stack);
    printf("Time elapsed by %d coroutine: %ld.%06lds.\n", ind, sum.tv_sec, sum.tv_usec);
    while (true)
    {
        bool all_finished = true;
        for (int i = 0; i < n; i++)
        {
            if (!is_finished[i])
            {
                all_finished = false;
                break;
            }
        }
        if (all_finished) {
            return;
        }
        context_swap(ind, begin, end, sum);
    }
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


int* merge_two_arrays()
{
    int size = int_file_array_size[0] + int_file_array_size[1];
    int* merged_array = (int*) malloc(size * sizeof(int));
    check_malloc(merged_array);
    merge_arrays(int_file_array[0], int_file_array[1], int_file_array_size[0], int_file_array_size[1], merged_array);
    free(int_file_array[0]);
    free(int_file_array[1]);
    return merged_array;
}

void final_merge() {
    FILE* final_file = fopen("final", "w+");
    check_fopen(final_file);
    int** merged_arrays;
    if (n > 2) {
        merged_arrays = (int**) malloc((n - 1) * sizeof(int *));
        check_malloc(merged_arrays);
        int size = int_file_array_size[0] + int_file_array_size[1];
        int size_before;
        merged_arrays[0] = merge_two_arrays();

        for (int i = 2; i < n; i++) {
            size_before = size;
            size += int_file_array_size[i];
            merged_arrays[i - 1] = (int*) malloc(size * sizeof(int));
            check_malloc(merged_arrays[i - 1]);
            merge_arrays(merged_arrays[i - 2], int_file_array[i], size_before, int_file_array_size[i], merged_arrays[i - 1]);
            free(int_file_array[i]);
            free(merged_arrays[i - 2]);
        }
        for (int i = 0; i < size; i++)
            fprintf(final_file, "%d ", merged_arrays[n - 2][i]);
    }
    else {
        int* merged_array = merge_two_arrays();
        int size = int_file_array_size[0] + int_file_array_size[1];
        for (int i = 0; i < size; i++) {
            fprintf(final_file, "%d ", merged_array[i]);
        }
        free(merged_array);
    }

    check_fclose(fclose(final_file));
}

void parse_file(FILE* f, int i) {
    int t;
    int* tmp;
    int j = 0, size = 1000;
    fscanf(f, "%d", &t);
    int_file_array[i] = (int*) malloc(1000 * sizeof(int));
    check_malloc(int_file_array[i]);
    int_file_array[i][j] = t;
    while (!feof(f)) {
        fscanf(f, "%d", &t);
        j++;
        if (j >= size) {
            tmp = realloc(int_file_array[i], size * 2 * sizeof(int));
            check_realloc(tmp);
            size *= 2;
            int_file_array[i] = tmp;
            tmp = NULL;
        }
        int_file_array[i][j] = t;
    }
    tmp = realloc(int_file_array[i], j * sizeof(int));
    check_realloc(tmp);
    int_file_array[i] = tmp;
    tmp = NULL;
    int_file_array_size[i] = j;
}

static char* allocate_stack()
{
    char* stack = (char*) malloc(stack_size * sizeof(char));
    check_malloc(stack);
    stack_t s;
    s.ss_sp = stack;
    s.ss_size = stack_size * sizeof(char);
    s.ss_flags = 0;
    if (sigaltstack(&s, NULL) == -1)
        handle_error("sigaltstack()");
    return stack;
}

int main(int argc, char* argv[]) {
    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    n = argc - 1;
    if (n == 1)
    {
        printf("You need more than 1 file! \n");
        return -1;
    }
    else if (n <= 0)
    {
        printf("Enter a valid number of files! \n");
        return -1;
    }

    FILE** fin = (FILE**) malloc(n * sizeof(FILE*));    //array of all input files
    check_malloc(fin);
    for (int i = 0; i < n; i++) {
        fin[i] = fopen(argv[i + 1], "r");
        check_fopen(fin[i]);
    }

    contexts = (ucontext_t*) malloc(n * sizeof(ucontext_t));
    is_finished = (bool*) malloc(n * sizeof(bool));
    check_malloc(contexts);
    check_malloc(is_finished);
    for (int i = 0; i < n; i++) {
        if (getcontext(&contexts[i]) == -1)
            handle_error("getcontext()");
        is_finished[i] = false;
        contexts[i].uc_stack.ss_sp = allocate_stack();
        contexts[i].uc_stack.ss_size = stack_size * sizeof(char);
        if (i == 0)
            contexts[i].uc_link = &context_main;
        else
            contexts[i].uc_link = &contexts[i - 1];
    }

    int_file_array = (int**) malloc(n * sizeof(int*));           //arrays of all numbers from all files
    int_file_array_size = (int*) malloc(n * sizeof(int));        //all sizes of arrays
    check_malloc(int_file_array);
    check_malloc(int_file_array_size);

    for (int i = 0; i < n; i++) {
        parse_file(fin[i], i);
        makecontext(&contexts[i], context_function, 3, 0, int_file_array_size[i] - 1, i);
    }

    if (swapcontext(&context_main, &contexts[n - 1]) == -1)
        handle_error("swapcontext() main to n - 1");

    final_merge();

    for (int i = 0; i < n; i++) {
        check_fclose(fclose(fin[i]));
    }
    free(int_file_array_size);
    free(contexts);
    free(is_finished);
    free(fin);
    gettimeofday(&end, NULL);
    printf("Time elapsed by the whole program: %ld.%06lds.\n", end.tv_sec - begin.tv_sec, end.tv_usec - begin.tv_usec);
    return 0;
}