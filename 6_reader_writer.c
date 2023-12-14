#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_READERS 3
#define NUM_WRITERS 2

int shared_data = 0;
int readers_count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t writer_sem, reader_sem;

void *reader(void *arg) {
    int reader_id = *((int *)arg);

    while (1) {
        sem_wait(&reader_sem);
        pthread_mutex_lock(&mutex);
        readers_count++;

        if (readers_count == 1) {
            sem_wait(&writer_sem);
        }

        pthread_mutex_unlock(&mutex);
        sem_post(&reader_sem);

        // Reading shared data
        printf("Reader %d read: %d\n", reader_id, shared_data);

        sem_wait(&reader_sem);
        pthread_mutex_lock(&mutex);
        readers_count--;

        if (readers_count == 0) {
            sem_post(&writer_sem);
        }

        pthread_mutex_unlock(&mutex);
        sem_post(&reader_sem);

        // Sleep to simulate processing time
        usleep(rand() % 1000000);
    }

    return NULL;
}

void *writer(void *arg) {
    int writer_id = *((int *)arg);

    while (1) {
        sem_wait(&writer_sem);
        
        // Writing to shared data
        shared_data++;
        printf("Writer %d wrote: %d\n", writer_id, shared_data);

        sem_post(&writer_sem);

        // Sleep to simulate processing time
        usleep(rand() % 1000000);
    }

    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS], writers[NUM_WRITERS];
    int reader_ids[NUM_READERS], writer_ids[NUM_WRITERS];

    sem_init(&reader_sem, 0, 1);
    sem_init(&writer_sem, 0, 1);

    // Create reader threads
    for (int i = 0; i < NUM_READERS; i++) {
        reader_ids[i] = i + 1;
        pthread_create(&readers[i], NULL, reader, &reader_ids[i]);
    }

    // Create writer threads
    for (int i = 0; i < NUM_WRITERS; i++) {
        writer_ids[i] = i + 1;
        pthread_create(&writers[i], NULL, writer, &writer_ids[i]);
    }

    // Join threads
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    // Clean up
    sem_destroy(&reader_sem);
    sem_destroy(&writer_sem);

    return 0;
}
