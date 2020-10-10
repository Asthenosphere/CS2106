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

void fizzbuzz_init ( int n ) {
    count = n;
    num_sem = malloc(sizeof(sem_t));
    fizz_sem = malloc(sizeof(sem_t));
    buzz_sem = malloc(sizeof(sem_t));
    fizzbuzz_sem = malloc(sizeof(sem_t));
    if (num_sem == NULL || fizz_sem == NULL || buzz_sem == NULL || fizzbuzz_sem == NULL) {
        fprintf(stderr, "Error allocating memory for semaphores.\n");
        exit(1);
    }
    sem_init(num_sem, 0, 1);
    sem_init(fizz_sem, 0, 0);
    sem_init(buzz_sem, 0, 0);
    sem_init(fizzbuzz_sem, 0, 0);
}

void num_thread( int n, void (*print_num)(int) ) {
    for (int i = 1; i <= n + 1; i++) {
        if (i == n + 1) {
            return;
        }
        sem_wait(num_sem);
        if (i % 3 == 0 && i % 5 == 0) {
            sem_post(fizzbuzz_sem);
        } else if (i % 5 == 0) {
            sem_post(buzz_sem);
        } else if (i % 3 == 0) {
            sem_post(fizz_sem);
        } else {
            print_num(i);
            sem_post(num_sem);
        }
    }
}

void fizz_thread( int n, void (*print_fizz)(void) ) {
    for (int i = 1; i <= n; i++) {
        if (i % 3 == 0) {
            sem_wait(fizz_sem);
            print_fizz();
            sem_post(num_sem);
        }
    }
}

void buzz_thread( int n, void (*print_buzz)(void) ) {
    for (int i = 1; i <= n; i++) {
        if (i % 5 == 0) {
            sem_wait(buzz_sem);
            print_buzz();
            sem_post(num_sem);
        }
    }
}

void fizzbuzz_thread( int n, void (*print_fizzbuzz)(void) ) {
    for (int i = 1; i <= n; i++) {
        if (i % 3 == 0 && i % 5 == 0) {
            sem_wait(fizzbuzz_sem);
            print_fizzbuzz();
            sem_post(num_sem);
        }
    }
}

void fizzbuzz_destroy() {
    free(num_sem);
    free(fizz_sem);
    free(buzz_sem);
    free(fizzbuzz_sem);
}
