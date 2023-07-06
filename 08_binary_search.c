#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 100

int binarySearch(int l, int r, int key, int n, int * a);

int main() {
    int * vector;
    vector = malloc(N * sizeof(int));

    int key = 0;

    do {
        printf("Please, enter the key: ");
        scanf("%d", &key);
    } while (key > (N - 1) || key < 0);
    
    for (size_t i = 0; i < N; i++)
        vector[i] = i;
    
    printf("Vector: ");
    for (int i = 0; i < N; i++)
        printf("%d ", vector[i]);
    
    printf("\nBinary Search result with key %d is: %d", key, binarySearch(0, N, key, N, vector));
    
    printf("\n");
    return 0;
}

int binarySearch(int l, int r, int key, int n, int * a) {
    
    int index = -1;
    int size = (r - l + 1) / n;
    
    if (size == 0 || n == 1) {
        int p = 0;
        #pragma omp parallel for
        for (int i = l; i <= r; i++) {
            if (a[i] == key)
                index = i;
        }
        return index;
    }

    int left = l;
    int right = r;
    
    omp_set_num_threads(n);
    
    /**
     * Quando si chiama omp_set_nested(1) con il valore 1 come argomento,
     * si imposta l'abilitazione delle regioni parallele annidate.
     * Ciò significa che è possibile creare regioni parallele all'interno di
     * altre regioni parallele e i thread possono essere creati per
     * l'esecuzione di queste regioni annidate. 
    */
    omp_set_nested(1);
    
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int lt = l + id * size;
        int rt = lt + size - 1;
        if (id == n - 1)
            rt = r;
        if (a[lt] <= key && a[rt] >= key) {
            left = lt;
            right = rt;
        }
    }

    if (left == l && right == r) {
        return -1;
    }
    
    return binarySearch(left, right, key, n, a);
}