#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include <time.h>
#define SIZE 1000

void printV(int v[], double times[], int n){
    double sum = 0;
    if(SIZE < 1000){
        for (int i = 0; i < SIZE; i++){
            printf("%d ", v[i]);
        }   
    }
    else{
        printf("Array passa das 1000 casas, print removido para melhor visualização, ordenação funcionando normalmente");
        printf("\n");   
    }
    
    printf("+----------------------------------------+\n");
    printf("| Execução          | Tempo (segundos)    \n");
    printf("+----------------------------------------+\n");

    for (int i = 0; i< n; i++){
        printf("| Execução %2d  | %12.6f s |\n", i+1, times[i]);
        sum += times[i];
    }

    printf("+----------------------------------------+\n");
    printf("| Tempo Total         | %12.6f s    |\n", sum);
    printf("| Média por Execução  | %12.6f s    |\n", sum/n);
    printf("+----------------------------------------+\n");
}

void merge(int v[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int *LVec = (int*) malloc(n1 * sizeof(int));
    int *RVec = (int*) malloc(n2 * sizeof(int));

    if (LVec == NULL || RVec == NULL) {
        fprintf(stderr, "Erro ao alocar memória\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < n1; i++)
        LVec[i] = v[l + i];
    for (j = 0; j < n2; j++)
        RVec[j] = v[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (LVec[i] <= RVec[j]) {
            v[k] = LVec[i];
            i++;
        }
        else {
            v[k] = RVec[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        v[k] = LVec[i];
        i++;
        k++;
    }

    while (j < n2) {
        v[k] = RVec[j];
        j++;
        k++;
    }

    free(LVec);
    free(RVec);
}

void merge_sort(int v[], int l, int r){
    if(l < r){
        int m = l + (r - l) / 2;

        merge_sort(v, l, m);
        merge_sort(v, m+1, r);

        merge(v, l, m, r);
    }
}

void mergesort_paralelo(int v[], int n) {
    int act;
    int l;

    for (act = 1; act <= n-1; act = 2*act) {
        #pragma omp parallel for private(l) shared(v)
        for (l = 0; l < n-1; l += 2*act) {
            int m = l + act - 1;
            if (m >= n-1)
                m = n-1;

            int r = (l + 2*act - 1 < n-1) ? l + 2*act - 1 : n-1;

            merge(v, l, m, r);
        }
    }
}

void iterador_merge_recursiv(void (*sort_func)(int[], int, int), int v[], int n, double *times) {
    clock_t start, end;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < n; j++) {
            v[j] = rand() % 100000;
        }
        start = clock();
        sort_func(v, 0, n - 1);
        end = clock();
        times[i] = ((double) (end - start)) / CLOCKS_PER_SEC;
    }
}

void iterador_merge_paralelo(void (*sort_func)(int[], int), int v[], int n, double *times) {
    clock_t start, end;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < n; j++) {
            v[j] = rand() % 100000;
        }
        start = clock();
        sort_func(v, n);
        end = clock();
        times[i] = ((double) (end - start)) / CLOCKS_PER_SEC;
    }
}

int main(void){
    srand(time(NULL));
    int v[SIZE];
    double times_recursive[10];
    double times_iterative[10];

    printf("Merge Sort Recursivo:\n");
    iterador_merge_recursiv(merge_sort, v, SIZE, times_recursive);
    printV(v, times_recursive, 10);

    printf("\nMerge Sort Iterativo:\n");
    iterador_merge_paralelo(mergesort_paralelo, v, SIZE, times_iterative);
    printV(v, times_iterative, 10);

    return 0;
}
