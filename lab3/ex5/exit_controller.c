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
    exit_controller->line_empty = 1;
    exit_controller->no_of_priorities = no_of_priorities;
    if (exit_controller->mutex == NULL) {
        fprintf(stderr, "Error allocating memory for semaphores.\n");
        exit(1);
    }
    for (int i = 0; i < 50; i++) {
        exit_controller->locks[i] = malloc(sizeof(sem_t));
        if (exit_controller->locks[i] == NULL) {
            fprintf(stderr, "Error allocating memory for semaphores.\n");
            exit(1);
        }
        sem_init(exit_controller->locks[i], 0, 0);
        exit_controller->frequencies[i] = 0;
    }
    sem_init(exit_controller->mutex, 0, 1);
}

void exit_controller_wait(exit_controller_t *exit_controller, int priority) {
    sem_wait(exit_controller->mutex);
    if (exit_controller->line_empty) {
        int is_higher_priority_waiting = 0;
        for (int i = 0; i < priority; i++) {
            if (exit_controller->frequencies[i] > 0) {
                is_higher_priority_waiting = 1;
                break;
            }
        }
        if (is_higher_priority_waiting) {
            exit_controller->frequencies[priority]++;
            sem_post(exit_controller->mutex);
            sem_wait(exit_controller->locks[priority]);
        } else {
            exit_controller->line_empty = 0;
            sem_post(exit_controller->mutex);
        }
    } else {
        sem_post(exit_controller->mutex);
        while (!exit_controller->line_empty) {
            int is_higher_priority_waiting = 0;
            for (int i = 0; i < priority; i++) {
                if (exit_controller->frequencies[i] > 0) {
                    is_higher_priority_waiting = 1;
                    break;
                }
            }
            if (is_higher_priority_waiting) {
                exit_controller->frequencies[priority]++;
                sem_wait(exit_controller->locks[priority]);
            } else {
                exit_controller->frequencies[priority]++;
                sem_wait(exit_controller->locks[priority]);
            }
        }
    }
}

void exit_controller_post(exit_controller_t *exit_controller, int priority) {
    sem_wait(exit_controller->mutex);
    exit_controller->line_empty = 1;
    int next_highest_priority_index = -1;
    for (int i = 0; i < priority; i++) {
        if (exit_controller->frequencies[i] > 0) {
            next_highest_priority_index = i;
            break;
        }
    }
    if (next_highest_priority_index != -1) {
        exit_controller->frequencies[next_highest_priority_index]--;
        sem_post(exit_controller->locks[next_highest_priority_index]);
    } else {
        next_highest_priority_index = -1;
        for (int i = priority; i < 50; i++) {
            if (exit_controller->frequencies[i] > 0) {
                next_highest_priority_index = i;
                break;
            }
        }
        if (next_highest_priority_index != -1) {
            exit_controller->frequencies[next_highest_priority_index]--;
            sem_post(exit_controller->locks[next_highest_priority_index]);
        }
    }
    sem_post(exit_controller->mutex);
}

void exit_controller_destroy(exit_controller_t *exit_controller){
    for (int i = 0; i < 50; i++) {
        free(exit_controller->locks[i]);
    }
    free(exit_controller->mutex);
}
