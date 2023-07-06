#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <limits.h>

void alloc_matrix(int *** matrix, int row, int col);
void fill_matrix(int ** matrix, int row, int col, int max_element);
void print_matrix(int ** matrix, int row, int col);
void dealloc_matrix(int ** matrix, int row);

int main() {
    srand(time(NULL));
    int ** matrix;
    int n;
    printf("Inserire il size della matrice quadrata: ");
    scanf("%d", &n);
    alloc_matrix(&matrix, n, n);
    fill_matrix(matrix, n, n, 10);
    print_matrix(matrix, n, n);

    int x;
    int max_diag = INT_MIN;
    #pragma omp parallel shared(matrix, n) private(x)
    {
        #pragma omp for reduction(max:max_diag)
        for (size_t i = 0; i < n; ++i) {
            x = matrix[i][i];
            if (max_diag < x) {
                max_diag = x;
            }
        }

        // Aspetto
        #pragma omp barrier

        // Controllo se i thread hanno finito, nel caso stampo il massimo
        #pragma omp master
        {
            printf("Il massimo è: %d", max_diag);
        }
    }

    dealloc_matrix(matrix, n);

    return 0;
}

void alloc_matrix(int *** matrix, int row, int col) {
    *matrix = malloc(row * sizeof(int *));
    #pragma omp parallel for shared(matrix, row, col)
    for (size_t i = 0; i < row; ++i) {
        (*matrix)[i] = malloc(col * sizeof(int));
    }
}

void fill_matrix(int ** matrix, int row, int col, int max_element) {
    #pragma omp parallel for shared(matrix, row, col)
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
    #pragma omp parallel for shared(matrix, row)
    for (size_t i = 0; i < row; ++i) {
        free(matrix[i]);
    }
    free(matrix);
}