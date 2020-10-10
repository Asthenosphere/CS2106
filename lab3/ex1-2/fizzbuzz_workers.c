/**
 * CS2106 AY 20/21 Semester 1 - Lab 3
 *
 * Your implementation should go in this file.
 */
#include "fizzbuzz_workers.h"
#include "barrier.h" // you may use barriers if you think it can help your
                     // implementation
#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>

// declare variables to be used here
int count;
sem_t *num_sem;
sem_t *fizz_sem;
sem_t *buzz_sem;
sem_t *fizzbuzz_sem;
barrier_t *barrier;

void fizzbuzz_init ( int n ) {
    count = n;
    num_sem = malloc(sizeof(sem_t));
    fizz_sem = malloc(sizeof(sem_t));
    buzz_sem = malloc(sizeof(sem_t));
    fizzbuzz_sem = malloc(sizeof(sem_t));
    barrier = malloc( sizeof(barrier_t));
    barrier_init(barrier, 4);
    if (num_sem == NULL || fizz_sem == NULL || buzz_sem == NULL || fizzbuzz_sem == NULL) {
        fprintf(stderr, "Error allocating memory for semaphores.\n");
        exit(1);
    }
    sem_init(num_sem, 0, 0);
    sem_init(fizz_sem, 0, 0);
    sem_init(buzz_sem, 0, 0);
    sem_init(fizzbuzz_sem, 0, 0);
}

void num_thread( int n, void (*print_num)(int) ) {
    for (count = 1; count <= n; count++) {
        if (count % 3 == 0 && count % 5 == 0) {
            sem_post(fizzbuzz_sem);
            sem_wait(num_sem);
        } else if (count % 5 == 0) {
            sem_post(buzz_sem);
            sem_wait(num_sem);
        } else if (count % 3 == 0) {
            sem_post(fizz_sem);
            sem_wait(num_sem);
        } else {
            print_num(count);
        }
    }
    sem_post(fizz_sem);
    sem_post(buzz_sem);
    sem_post(fizzbuzz_sem);
    barrier_wait(barrier);
}

void fizz_thread( int n, void (*print_fizz)(void) ) {
    while (1) {
        sem_wait(fizz_sem);
        if (count < n) {
            print_fizz();
            sem_post(num_sem);
        } else {
            break;
        }
    }
    barrier_wait(barrier);
}

void buzz_thread( int n, void (*print_buzz)(void) ) {
    while (1) {
        sem_wait(buzz_sem);
        if (count < n) {
            print_buzz();
            sem_post(num_sem);
        } else {
            break;
        }
    }
    barrier_wait(barrier);
}

void fizzbuzz_thread( int n, void (*print_fizzbuzz)(void) ) {
    while (1) {
        sem_wait(fizzbuzz_sem);
        if (count < n) {
            print_fizzbuzz();
            sem_post(num_sem);
        } else {
            break;
        }
    }
    barrier_wait(barrier);
}

void fizzbuzz_destroy() {
    free(num_sem);
    free(fizz_sem);
    free(buzz_sem);
    free(fizzbuzz_sem);
    barrier_destroy(barrier);
}
