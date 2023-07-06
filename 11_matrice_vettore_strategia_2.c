#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

void alloc_matrix(int *** matrix, int row, int col);
void fill_matrix(int ** matrix, int row, int col, int max_element);
void print_matrix(int ** matrix, int row, int col);
void dealloc_matrix(int ** matrix, int row);

int main() {
    srand(time(NULL));
    int row, col;
    printf("Inserire numero di righe: ");
    scanf("%d", &row);
    printf("Inserire numero di colonne: ");
    scanf("%d", &col);

    int ** matrix;
    alloc_matrix(&matrix, row, col);
    fill_matrix(matrix, row, col, 10);
    puts("Matrice:");
    print_matrix(matrix, row, col);

    int * x = malloc(col * sizeof(int));
    for (size_t i = 0; i < col; ++i) {
        x[i] = rand() % 20;
    }

    puts("Vettore x:");
    for (size_t i = 0; i < col; ++i) {
        printf("%d ", x[i]);
    }
    putchar('\n');

    int * y = calloc(row, sizeof(int));
    int nloc = 0, step, remainder, tid, num_threads;
    #pragma omp parallel shared(matrix, x, y, row, col, num_threads) private(nloc, step, remainder, tid) num_threads(2)
    {
        tid = omp_get_thread_num();
        num_threads = omp_get_num_threads();
        nloc = col / num_threads;
        remainder = col % num_threads;
        if (tid < remainder) {
            nloc++;
            step = 0;
        } else {
            step = remainder;
        }

        for (size_t j = 0; j < nloc; ++j) {
            size_t index = j + tid * nloc + step;
            for (size_t i = 0; i < row; ++i) {
                y[i] += matrix[i][index] * x[index];
            }
        }
    }

    puts("Vettore y:");
    for (size_t i = 0; i < row; ++i) {
        printf("%d ", y[i]);
    }
    putchar('\n');

    dealloc_matrix(matrix, row);

    return 0;
}

void alloc_matrix(int *** matrix, int row, int col) {
    *matrix = malloc(row * sizeof(int *));

    int tid, num_threads, step, remainder, nloc = 0;
    #pragma omp parallel shared(matrix, num_threads, row, col) private(tid, step, remainder, nloc)
    {
        tid = omp_get_thread_num();
        num_threads = omp_get_num_threads();
        nloc = row / num_threads;
        remainder = row % num_threads;

        if (tid < remainder) {
            nloc++;
            step = 0;
        } else {
            step = remainder;
        }

        for (size_t i = 0; i < nloc; ++i) {
            size_t index = i + tid * nloc + step;
            (*matrix)[index] = malloc(col * sizeof(int));
        }
    }
}

void fill_matrix(int ** matrix, int row, int col, int max_element) {
    #pragma omp parallel for collapse(2)
    for (size_t i = 0; i < row; ++i) {
        for (size_t j = 0; j < col; ++j) {
            matrix[i][j] = rand() % max_element;
        }
    }
}

void print_matrix(int ** matrix, int row, int col) {
    for (size_t i = 0; i < row; ++i) {
        for (size_t j = 0; j < col; ++j) {
            printf("%d ", matrix[i][j]);
        }
        putchar('\n');
    }
}

void dealloc_matrix(int ** matrix, int row) {
    #pragma omp parallel for
    for (size_t i = 0; i < row; ++i) {
        free(matrix[i]);
    }
    free(matrix);
}