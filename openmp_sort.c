#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define TOTAL_NUMBERS 100000
#define NUMBER_OF_BUCKETS 10
#define HIGHEST_POSSIBLE_NUMBER 10000






typedef struct {
    int* numbers;
    int count;
} Bucket;



Bucket buckets[NUMBER_OF_BUCKETS];
void prepareBuckets() {
    for (int index = 0; index < NUMBER_OF_BUCKETS; index++) {
        buckets[index].numbers = (int*)malloc(TOTAL_NUMBERS * 
sizeof(int));
        buckets[index].count = 0;
    }
}



int sortFunction(const void* x, const void* y) {
    return (*(int*)x - *(int*)y);
}




void parallelBucketSort(int data[], int size) {
    int i, j, rangePerBucket = HIGHEST_POSSIBLE_NUMBER / 
NUMBER_OF_BUCKETS;

    prepareBuckets();

    #pragma omp parallel for shared(buckets)
    for (i = 0; i < size; i++) {
        int index = data[i] / rangePerBucket;
        if (index >= NUMBER_OF_BUCKETS) index = NUMBER_OF_BUCKETS - 1;
        #pragma omp critical
        buckets[index].numbers[buckets[index].count++] = data[i];
    }



    #pragma omp parallel for
    for (i = 0; i < NUMBER_OF_BUCKETS; i++) {
        qsort(buckets[i].numbers, buckets[i].count, sizeof(int), 
sortFunction);
    }
    


    j = 0;
    for (i = 0; i < NUMBER_OF_BUCKETS; i++) {
        for (int k = 0; k < buckets[i].count; k++) {
            data[j++] = buckets[i].numbers[k];
        }
    }
}




void displayArray(int data[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
}




int main() {
    int data[TOTAL_NUMBERS];
    srand(time(NULL));

    for (int i = 0; i < TOTAL_NUMBERS; i++) {
        data[i] = rand() % HIGHEST_POSSIBLE_NUMBER;
    }

    printf("Original array:\n");
    displayArray(data, TOTAL_NUMBERS);

    double startTime = omp_get_wtime();
    parallelBucketSort(data, TOTAL_NUMBERS);
    double endTime = omp_get_wtime();
    double elapsedTime = (endTime - startTime) * 1000;

    printf("\nArray after sorting:\n");
    displayArray(data, TOTAL_NUMBERS);
    printf("\nTotal time: %f ms\n", elapsedTime);

    for (int i = 0; i < NUMBER_OF_BUCKETS; i++) {
        free(buckets[i].numbers);
    }

    return 0;
}

