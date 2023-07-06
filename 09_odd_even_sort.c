#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define SIZE 10
#define MAXVALUE 100

void print_array(int * array, int size);
void fill_array(int * array, int size, int max_element);
void oddEvenSort(int * array, int size);

int main() {
    int * array;
    array = malloc(SIZE * sizeof(int));
    srand(time(NULL));

    fill_array(array, SIZE, MAXVALUE);
    print_array(array, SIZE);
    oddEvenSort(array, SIZE);
    print_array(array, SIZE);
    
    return 0;
}

void print_array(int * array, int size) {
    for (size_t i = 0; i < size; ++i) {
        printf("%d ", array[i]);
    }
    putchar('\n');
}

void fill_array(int * array, int size, int max_element) {
	for (size_t i = 0; i < size; ++i) {
		array[i] = rand() % max_element;
	}
}

void oddEvenSort(int * array, int size) {
    int sw = 1, start = 0, i = 0;
    int temp;
    while (sw || start) {
        sw = 0 ;
        
        #pragma omp parallel for private(temp) 
        for (i = start ; i < size - 1 ; i += 2) {
            if (array[i] > array[i + 1]) {
                temp = array[i];
                array[i] = array[i +1];
                array[i + 1] = temp;
                sw = 1;
            }
        }

        if (start == 0) {
            start = 1;
        } else {
            start = 0;
        }
    }
}