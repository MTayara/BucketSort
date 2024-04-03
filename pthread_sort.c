#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define ARRAY_SIZE 10000
#define BUCKET_COUNT 10
#define MAX_VALUE 1000
#define THREAD_COUNT BUCKET_COUNT





typedef struct {
    int* bucket;
    int count;
} Bucket;

typedef struct {
    int* array;
    int size;
    int index;
} ThreadArg;






Bucket buckets[BUCKET_COUNT];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void initializeBuckets() {
    for (int i = 0; i < BUCKET_COUNT; i++) {
        buckets[i].bucket = (int*)malloc(ARRAY_SIZE * sizeof(int));
        buckets[i].count = 0;
    }
}

int compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}



void* sortBucket(void* arg) {
    ThreadArg* tArg = (ThreadArg*)arg;
    qsort(buckets[tArg->index].bucket, buckets[tArg->index].count, sizeof(int), compare);
    pthread_exit(NULL);
}



void parallelBucketSort(int arr[], int n) {
    int i, j, bucketRange = MAX_VALUE / BUCKET_COUNT;
    pthread_t threads[THREAD_COUNT];
    ThreadArg threadArgs[THREAD_COUNT];



    initializeBuckets();
    for (i = 0; i < n; i++) {
        int bucketIndex = arr[i] / bucketRange;
        if (bucketIndex >= BUCKET_COUNT) bucketIndex = BUCKET_COUNT - 1;
        pthread_mutex_lock(&mutex);
        buckets[bucketIndex].bucket[buckets[bucketIndex].count++] = arr[i];
        pthread_mutex_unlock(&mutex);
    }



    for (i = 0; i < THREAD_COUNT; i++) {
        threadArgs[i].array = buckets[i].bucket;
        threadArgs[i].size = buckets[i].count;
        threadArgs[i].index = i;
        pthread_create(&threads[i], NULL, sortBucket, (void*)&threadArgs[i]);
    }

    for (i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    j = 0;
    for (i = 0; i < BUCKET_COUNT; i++) {
        for (int k = 0; k < buckets[i].count; k++) {
            arr[j++] = buckets[i].bucket[k];
        }
    }
}


void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}




int main() {
    int arr[ARRAY_SIZE];
    srand(time(NULL));



    for (int i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = rand() % MAX_VALUE;
    }

    printf("Unsorted array:\n");
    printArray(arr, ARRAY_SIZE);

    clock_t start = clock();
    parallelBucketSort(arr, ARRAY_SIZE);


    clock_t end = clock();

    double time_taken = ((double)end - start) / CLOCKS_PER_SEC * 1000;



    printf("\nSorted array:\n");
    printArray(arr, ARRAY_SIZE);
    printf("\nTotal time: %f ms\n", time_taken);

    for (int i = 0; i < BUCKET_COUNT; i++) {
        free(buckets[i].bucket);
    }

    return 0;
}

