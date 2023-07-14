#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

void alloc_matrix(int *** matrix, int row, int col);
void fill_matrix(int ** matrix, int row, int col, int max_element);
void print_matrix(int ** matrix, int row, int col);
void dealloc_matrix(int ** matrix, int row);
void fill_array(int * array, int size, int max_element);
void print_array(int * array, int size);

int main() {
	srand(time(NULL));

	size_t i, j; // Indici
	double startTime, endTime; // Tempo di inizio e fine del programma

	int ** A; // Matrice A
	int * a, * b;
	int N, M; // N = righe, M = colonne
	int max_element; // Numero massimo generabile all'interno della matrice
	int np; // Unità processanti

	printf("Inserisci il numero di righe: ");
	scanf("%d", &N);
	printf("Inserisci il numero di colonne: " );
	scanf("%d", &M);
	printf("Inserire il massimo numero generabile all'interno della matrice e dei vettori: ");
	scanf("%d", &max_element);
	printf("Inserire il numero di unità processanti: ");
	scanf("%d", &np);
	omp_set_num_threads(np);

	#pragma omp master
	{
		// Generazione e stampa della matrice A
		alloc_matrix(&A, N, M);
		fill_matrix(A, N, M, max_element + 1); // Numeri nell'intervallo [0, max_element]
		puts("Matrice A");
		print_matrix(A, N, M);
		putchar('\n'); // Newline per garantire una stampa visivamente corretta	

		// Generazione e stampa del vettore a
		if ((a = malloc(N * sizeof(int))) == NULL) {
			fprintf(stderr, "Error at allocating array \"a\"\n");
			dealloc_matrix(A, N);
			exit(-2);
		}
		
		fill_array(a, N, max_element + 1);
		puts("Vettore a");
		print_array(a, N);
		putchar('\n');

		// Generazione e stampa del vettore b
		if ((b = malloc(N * sizeof(int))) == NULL) {
			fprintf(stderr, "Error at allocating array \"b\"\n");
			dealloc_matrix(A, N);
			free(a);
			exit(-3);
		}

		fill_array(b, N, max_element + 1);
		puts("Vettore b");
		print_array(b, N);
		putchar('\n');
	}

	long alpha = 0; // Trattandosi di moltiplicazioni, potrebbe uscire un numero troppo grande per il tipo di dato "int"
	int beta = 0; // Somma degli elementi della matrice
	startTime = omp_get_wtime(); // Tempo di inizio
	#pragma omp parallel for schedule(dynamic) reduction(+:alpha)
	for (size_t i = 0; i < N; ++i) 
	{
		alpha += a[i] * b[i];
	}

	#pragma omp parallel for schedule(dynamic) reduction(+:beta)
	for (size_t i = 0; i < N; ++i) {
		for (size_t j = 0; j < M; ++j) {
			beta += A[i][j];
		}
	}

	#pragma omp master
	{
		// Tempo di fine
		endTime = omp_get_wtime();
		// Stampa di alpha e beta da parte del thread master
		printf("alpha + beta = %ld\n", alpha + beta);
		// Il master thread doveva stampare il tempo di esecuzione
		printf("Tempo di esecuzione: %lf\n", endTime - startTime );
	}

	dealloc_matrix(A, N);
	free(a);
	free(b);

	return 0;
}


void alloc_matrix(int *** matrix, int row, int col) {
    	if ((*matrix = malloc(row * sizeof(int *))) == NULL) {
		fprintf(stderr, "Error at allocating matrix per rows.\n");
		exit(-20);
	}
    	for (size_t i = 0; i < row; ++i) {
        	for (size_t j = 0; j < col; ++j) {
            		if (((*matrix)[i] = malloc(col * sizeof(int))) == NULL) {
				fprintf(stderr, "Error at allocating matrix per columns.\n");
				exit(-21);
			}
        	}
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

void fill_array(int * array, int size, int max_element) {
	for (size_t i = 0; i < size; ++i) {
		array[i] = rand() % max_element;
	}
}

void print_array(int * array, int size) {
    for (size_t i = 0; i < size; ++i) {
        printf("%d ", array[i]);
    }
}
