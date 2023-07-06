/* Implementare un programma parallelo per l’ambiente multicore con np unità processanti 
 * che impieghi la libreria OpenMP. Il programma deve essere organizzato come segue:
 ** il core master deve generare e leggere una matrice A di dimensione M×N
 ** i core devono organizzarsi per estrarre ognuno una sottomatrice, decomponendo la matrice A per 
  *  blocchi riga, e costruendo la trasposta di tale sottomatrice, in una matrice Bloc
 ** infine i core devono collaborare per sommare le sotto matrici estratte Bloc in un’unica matrice finale C
 ** il core master stampa la matrice risultato e il tempo d’esecuzione. */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define M_BLOC n // righe di Bloc
#define N_BLOC m // colonne di Bloc
#define M_C (m / n_threads) + remainder
#define N_C m
#define OFFSET(id) id * (M_BLOC / n_threads)

void alloc_matrix(int *** matrix, int row, int col);
void fill_matrix(int ** matrix, int row, int col, int max_element);
void print_matrix(int ** matrix, int row, int col);
void dealloc_matrix(int ** matrix, int row);

int main(int argc, char * argv[]) {
    srand(time(NULL));
    if (argc != 4) {
        fprintf(stderr, "%s <M> <N> <t>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int m = atoi(argv[1]);
    int n = atoi(argv[2]);
    int n_threads = atoi(argv[3]);

    int ** matrix;
    int ** Bloc;
    alloc_matrix(&matrix, m, n);
    fill_matrix(matrix, m, n, 5);
    alloc_matrix(&Bloc, M_BLOC, N_BLOC);
    
    size_t i, j;
    double t1, t2;
    t1 = omp_get_wtime();
    
    #pragma omp master
    {
        print_matrix(matrix, m, n);
    }

    #pragma omp parallel for shared(matrix, m, n, Bloc) private(i, j)
    for (i = 0; i < m; ++i) {
        for (j = 0; j < n; ++j) {
            Bloc[j][i] = matrix[i][j];
        }
    }

    size_t b;
    int remainder = M_BLOC % n_threads;

    #pragma omp master
    {
        for (b = 0; b < n_threads; ++b) {
            printf("\nBloc #%d\n", b);
            int S = M_BLOC / n_threads;

            if (b == 0) {
                S += remainder;
            }

            for (i = 0; i < S; ++i) {
                for (j = 0; j < N_BLOC; ++j) { 
                    printf("%d ", Bloc[OFFSET(b) + i][j]);
                }
                putchar('\n');
            }
        }
    }

    int ** C;
    alloc_matrix(&C, M_C, N_C);
    #pragma omp parallel for shared(Bloc, remainder) private(i, j, b) num_threads(n_threads)
    for (i = 0; i < M_C - remainder; ++i) {
        for (j = 0; j < N_C; ++j) {
            for (b = 0; b < n_threads; ++b) {
                C[i][j] += Bloc[OFFSET(b) + i][j];
            }
        }
    }

    // Siccome ci potrebbe essere elementi esclusi dal for di sopra, con questo for vengono inclusi
    // però siccome si tratta di righe coperte solo dal primo thread, non ha senso fare la somma
    // e quindi c'è una semplice assegnazione
    #pragma omp parallel for shared(Bloc, remainder) private(i, j, b) num_threads(n_threads)
    for (i = M_C - remainder; i < M_C; ++i) {
        for (j = 0; j < N_C; ++j) {
            C[i][j] = Bloc[i][j];
        }
    }
    t2 = omp_get_wtime();
    #pragma omp master
    {
        puts("Matrice C:");
        print_matrix(C, M_C, N_C);
        printf("Tempo d'esecuzione: %lf\n", t2 - t1);
    }

    dealloc_matrix(matrix, m);
    dealloc_matrix(Bloc, M_BLOC);
    dealloc_matrix(C, M_C);

    return 0;
}

void alloc_matrix(int *** matrix, int row, int col) {
    *matrix = malloc(row * sizeof(int *));
    for (size_t i = 0; i < row; ++i) {
        (*matrix)[i] = malloc(col * sizeof(int));
    }
}

void fill_matrix(int ** matrix, int row, int col, int max_element) {
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
    for (size_t i = 0; i < row; ++i) {
        free(matrix[i]);
    }
    free(matrix);
}