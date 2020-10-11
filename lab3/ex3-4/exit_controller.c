/**
 * CS2106 AY 20/21 Semester 1 - Lab 3
 *
 * This file contains declarations. You should only modify the fifo_sem_t struct,
 * as the method signatures will be needed to compile with the runner.
 */
#include <stdlib.h>
#include <stdio.h>
#include "exit_controller.h"

void exit_controller_init(exit_controller_t *exit_controller, int no_of_priorities) {
    exit_controller->mutex = malloc(sizeof(sem_t));
    exit_controller->high_priority_mutex = malloc(sizeof(sem_t));
    exit_controller->low_priority_mutex = malloc(sizeof(sem_t));
    exit_controller->line_empty = 1;
    exit_controller->high_priority_count = 0;
    if (exit_controller->mutex == NULL ||
        exit_controller->high_priority_mutex == NULL ||
        exit_controller->low_priority_mutex == NULL) {
        fprintf(stderr, "Error allocating memory for semaphores.\n");
        exit(1);
    }
    sem_init(exit_controller->mutex, 0, 1);
    sem_init(exit_controller->high_priority_mutex, 0, 0);
    sem_init(exit_controller->low_priority_mutex, 0, 0);
}

void exit_controller_wait(exit_controller_t *exit_controller, int priority) {
    sem_wait(exit_controller->mutex);
    if (exit_controller->line_empty) {
        exit_controller->line_empty = 0;
        sem_post(exit_controller->mutex);
    } else {
        sem_post(exit_controller->mutex);
        while (!exit_controller->line_empty) {
            if (priority == 0) {
                exit_controller->high_priority_count++;
                sem_wait(exit_controller->high_priority_mutex);
            } else {
                sem_wait(exit_controller->low_priority_mutex);
            }
        }

    }
}

void exit_controller_post(exit_controller_t *exit_controller, int priority) {
    sem_wait(exit_controller->mutex);
    exit_controller->line_empty = 1;
    if (exit_controller->high_priority_count > 0) {
        sem_post(exit_controller->high_priority_mutex);
    } else {
        sem_post(exit_controller->low_priority_mutex);
    }
    sem_post(exit_controller->mutex);
}

void exit_controller_destroy(exit_controller_t *exit_controller){
    free(exit_controller->low_priority_mutex);
    free(exit_controller->high_priority_mutex);
}
