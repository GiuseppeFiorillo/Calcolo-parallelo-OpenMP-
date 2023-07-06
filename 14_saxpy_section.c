#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

void alloc_matrix(int *** matrix, int row, int col);
void fill_matrix(int ** matrix, int row, int col, int max_element);
void print_matrix(int ** matrix, int row, int col);
void dealloc_matrix(int ** matrix, int row);

int main(int argc, char * argv[]) {
    srand(time(NULL));
    if (argc != 6) {
        fprintf(stderr, "Correct usage: %s <num_threads> <row> <col> <alpha> <beta>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n_threads = atoi(argv[1]);
    int row = atoi(argv[2]);
    int col = atoi(argv[3]);
    int alpha = atoi(argv[4]);
    int beta = atoi(argv[5]);

    int ** A;
    alloc_matrix(&A, row, col);
    fill_matrix(A, row, col, 10);
    puts("Matrice:");
    print_matrix(A, row, col);

    int * b = malloc(col * sizeof(int));
    int * a = malloc(row * sizeof(int));
    int * vet_first = malloc(row * sizeof(int));
    int * vet_second = malloc(row * sizeof(int));
    int * vet_res = malloc(row * sizeof(int));

    puts("b:");
    #pragma omp parallel shared(b, col) num_threads(n_threads)
    {
        #pragma omp for
        for (size_t i = 0; i < col; ++i) {
            b[i] = rand() % 5 + 1;
        }
        #pragma omp barrier
        #pragma omp master
        for (size_t i = 0; i < col; ++i) {
            printf("%d ", b[i]);
        }
    } 
    putchar('\n');

    puts("a:");
    #pragma omp parallel shared(a, row) num_threads(n_threads)
    {
        #pragma omp for
        for (size_t i = 0; i < row; ++i) {
            a[i] = rand() % 5 + 1;
        }
        #pragma omp barrier
        #pragma omp master
        for (size_t i = 0; i < row; ++i) {
            printf("%d ", a[i]);
        }
    }
    putchar('\n');

    double t0, t1, t_res;
    int res = 1;
    t0 = omp_get_wtime();
    #pragma omp parallel sections shared(A, b, a, row, col, vet_first, vet_second, alpha, beta) num_threads(2)
    {  
        #pragma omp section
        {
            for (size_t i = 0; i < row; ++i) {
                for (size_t j = 0; j < col; ++j) {
                    vet_first[i] += alpha * A[i][j] * b[j];
                }
            }
        }
 
        #pragma omp section
        {
            for (size_t i = 0; i < row; ++i) {
                vet_second[i] = beta * a[i];
            }
        }
    }

    #pragma omp parallel for shared(vet_first, vet_second, row) reduction(+:vet_res[:row])
    for (size_t i = 0; i < row; ++i) {
        vet_res[i] = vet_first[i] + vet_second[i];
    }

    #pragma omp parallel for shared(vet_res, row) reduction(*: res) num_threads(n_threads)
    for (size_t i = 0; i < row; ++i) {
        res *= vet_res[i];
    }
    t1 = omp_get_wtime();
    t_res = t1 - t0;

    printf("Risultato: %d\n", res);
    printf("Tempo misurato: %lf\n", t_res);
    
    free(vet_res);
    free(a);
    free(b);
    dealloc_matrix(A, row);
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