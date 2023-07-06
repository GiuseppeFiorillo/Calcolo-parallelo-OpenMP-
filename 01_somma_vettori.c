#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

void printArray(double * array, int size);
void fillArray(double * array, int size, double max_element);

int main() {
    srand(time(NULL));
    double * a, * b, * c;
    int size;
    printf("Inserire il size dei due array: ");
    scanf("%d", &size);
    a = malloc(size * sizeof(double));
    b = malloc(size * sizeof(double));
    c = calloc(size, sizeof(double));
    fillArray(a, size, 4.0);
    fillArray(b, size, 4.0);
    puts("a:");
    printArray(a, size);
    putchar('\n');
    puts("b:");
    printArray(b, size);
    putchar('\n');

    size_t i;
    int nloc, remainder, step, id, n_threads;
    #pragma omp parallel shared(a, b, c, n_threads, size) private(i, nloc, remainder, step, id)
    {
        id = omp_get_thread_num();
        n_threads = omp_get_num_threads();
        nloc = size / n_threads;
        remainder = size % n_threads;

        if (id < remainder) {
            nloc++;
            step = 0;
        } else {
            step = remainder;
        }

        for (i = 0; i < nloc; ++i) {
            size_t index = i + id * nloc + step;
            c[index] = a[index] + b[index];
        }
    }

    puts("c:");
    printArray(c, size);
    putchar('\n');

    free(a);
    free(b);
    free(c);    

    return 0;
}

void printArray(double * array, int size) {
    for (size_t i = 0; i < size; ++i) {
        printf("%.3lf ", array[i]);
    }
}

void fillArray(double * array, int size, double max_element) {
    for (size_t i = 0; i < size; ++i) {
        array[i] = (double) rand() / (double) RAND_MAX * max_element;
    }
}