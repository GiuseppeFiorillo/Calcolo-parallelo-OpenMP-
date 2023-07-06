#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void print_matrix(int ** matrix, int row, int col);

int main() {
    int row, col;
    printf("Inserisci numero di righe: ");
    scanf("%d", &row);
    printf("Inserisci numero di colonne: ");
    scanf("%d", &col);
    int ** matrix = malloc(row * sizeof(int *));
    for (size_t i = 0; i < row; ++i) {
        matrix[i] = malloc(col * sizeof(int)); 
    }

    for (size_t i = 0; i < row; ++i) {
        for (size_t j = 0; j < col; ++j) {
            matrix[i][j] = rand() % 20;
        }
    }

    int scalar;
    printf("Inserisci scalare: ");
    scanf("%d", &scalar);

    puts("Prima:");
    print_matrix(matrix, row, col);
    size_t i, j;
    #pragma omp parallel for shared(row, col, matrix, scalar) private(i, j)
    for (i = 0; i < row; ++i) {
        for (j = 0; j < col; ++j) {
            matrix[i][j] *= scalar;
        }
    }

    puts("Dopo:");
    print_matrix(matrix, row, col);

    return 0;
}

void print_matrix(int ** matrix, int row, int col) {
    for (size_t i = 0; i < row; ++i) {
        for (size_t j = 0; j < col; ++j) {
            printf("%d ", matrix[i][j]);
        }
        putchar('\n');
    }
}