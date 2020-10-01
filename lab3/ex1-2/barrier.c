/**
 * CS2106 AY 20/21 Semester 1 - Lab 3
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */

#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include "barrier.h"

// Initialise barrier here
void barrier_init ( barrier_t *barrier, int count ) {
    barrier->count = count;
    barrier->mutex = malloc(sizeof(sem_t));
    barrier->s = malloc(sizeof(sem_t));
    if (barrier->mutex == NULL || barrier->s == NULL) {
        fprintf(stderr, "Error allocating memory for semaphore.\n");
        exit(1);
    }
    sem_init(barrier->mutex, 0, 1);
    sem_init(barrier->s, 0, 0);
}

void barrier_wait ( barrier_t *barrier ) {
    sem_wait(barrier->mutex);
    barrier->count--;
    printf("Count: %d\n", barrier->count);
    if (barrier->count == 0) {
        sem_post(barrier->s);
    }
    sem_post(barrier->mutex);
    printf("Before: %d\n", *(barrier->s));
    sem_wait(barrier->s);
    printf("After: %d\n", *(barrier->s));
    sem_post(barrier->s);
}

// Perform cleanup here if you need to
void barrier_destroy ( barrier_t *barrier ) {
    free(barrier->mutex);
    free(barrier->s);
}
