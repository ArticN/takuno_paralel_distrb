#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include <time.h>

#define SIZE 100000

void printV(int v[], double times[], int n){
    double sum = 0;
    if (SIZE < 1000) {
        for (int i = 0; i < SIZE; i++) {
            printf("%d ", v[i]);
        }   
    } else {
        printf("Array passa das 1000 casas, print removido para melhor visualização, ordenação funcionando normalmente\n");   
    }
    
    printf("+----------------------------------------+\n");
    printf("| Execução          | Tempo (segundos)   |\n");
    printf("+----------------------------------------+\n");

    for (int i = 0; i < n; i++) {
        printf("| Execução %2d  | %12.6f s |\n", i + 1, times[i]);
        sum += times[i];
    }

    printf("+----------------------------------------+\n");
    printf("| Tempo Total         | %12.6f s    |\n", sum);
    printf("| Média por Execução  | %12.6f s    |\n", sum / n);
    printf("+----------------------------------------+\n");
}

void save_file(double times_recursive[], double times_parallel[], int n) {
    FILE *file = fopen("resultados.txt", "w");
    if (file == NULL) {
        printf("Erro ao criar arquivo de resultados!\n");
        return;
    }

    double sum_recursive = 0, sum_parallel = 0;

    for (int i = 0; i < n; i++) {
        sum_recursive += times_recursive[i];
        sum_parallel += times_parallel[i];
    }

    double avg_recursive = sum_recursive / n;
    double avg_parallel = sum_parallel / n;
    double diff = avg_recursive - avg_parallel;

    fprintf(file, "+------------------------------------------------------+\n");
    fprintf(file, "| Execução          | Merge Recursivo  | Merge Paralelo |\n");
    fprintf(file, "+------------------------------------------------------+\n");

    for (int i = 0; i < n; i++) {
        fprintf(file, "| Execução %2d      | %12.6f s | %12.6f s |\n", i + 1, times_recursive[i], times_parallel[i]);
    }

    fprintf(file, "+------------------------------------------------------+\n");
    fprintf(file, "| Tempo Total       | %12.6f s | %12.6f s |\n", sum_recursive, sum_parallel);
    fprintf(file, "| Média por Execução| %12.6f s | %12.6f s |\n", avg_recursive, avg_parallel);
    fprintf(file, "+------------------------------------------------------+\n");
    fprintf(file, "| Diferença Média   | %12.6f s |\n", diff);
    fprintf(file, "+------------------------------------------------------+\n");

    fclose(file);
    printf("\nResultados salvos em 'resultados.txt'.\n");
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

void mergesort_recurs(int v[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;

        mergesort_recurs(v, l, m);
        mergesort_recurs(v, m + 1, r);

        merge(v, l, m, r);
    }
}

void mergesort_paralelo(int v[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        #pragma omp parallel
        {
            #pragma omp single
            {
                #pragma omp task
                mergesort_paralelo(v,l,m);
                
                #pragma omp task
                mergesort_paralelo(v, m+1, r);
            }
        }

        merge(v, l, m, r);
    }
}

void iterador_merge_recursiv(void (*sort_func)(int[], int, int), int v[], int n, double *times) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < n; j++) {
            v[j] = rand() % 100000;
        }
        double start = omp_get_wtime();
        sort_func(v, 0, n - 1);
        double end = omp_get_wtime();
        times[i] = end - start;
    }
}

void iterador_merge_paralelo(void (*sort_func)(int[], int, int), int v[], int n, double *times) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < n; j++) {
            v[j] = rand() % 100000;
        }
        double start = omp_get_wtime();
        sort_func(v, 0, n - 1);
        double end = omp_get_wtime();
        times[i] = end - start;
    }
}

int main(void) {
    srand(time(NULL));
    int v[SIZE];
    double times_recursive[10];
    double times_parallel[10];

    printf("Merge Sort Recursivo:\n");
    iterador_merge_recursiv(mergesort_recurs, v, SIZE, times_recursive);
    printV(v, times_recursive, 10);

    printf("\nMerge Sort Paralelo:\n");
    iterador_merge_paralelo(mergesort_paralelo, v, SIZE, times_parallel);
    printV(v, times_parallel, 10);

    save_file(times_recursive, times_parallel, 10);

    printf("O Número de Threads utilizadas foi %d\n", omp_get_max_threads());
    return 0;
}