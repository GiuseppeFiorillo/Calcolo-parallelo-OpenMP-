#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

void printArray(int * array, int n);

int main() {
	int N;
	printf("Inserire il numero di elementi: ");
	scanf("%d", &N);
	int * a = malloc(N * sizeof(int));
	int * b = malloc(N * sizeof(int));
    
	for (size_t i = 0; i < N; i++) {

		a[i] = i + 1;
		b[i] = i + 1;
	}

	printf("Primo array:\n");
	printArray(a, N);
	printf("Secondo array:\n");
	printArray(b, N);

	int t1, t2;
	unsigned long * tmp = malloc(N * sizeof(unsigned long));
	unsigned long sum = 0;
	t1 = omp_get_wtime();
	#pragma omp parallel for schedule(guided) reduction(+:sum)
	for (size_t i = 0; i < N; i++) 
	{
		sum += a[i] * b[i];
	}
	t2 = omp_get_wtime();
	printf("Risultato: %lu\n", sum);
	printf("Tempo: %d\n", t1 + t2);
	return 0;
}

void printArray(int * array, int n) {
	for (size_t i = 0; i < n; i++) {
		printf("[%d] = %d\n", i, array[i]);
	}
}