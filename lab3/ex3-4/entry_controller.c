/**
 * CS2106 AY 20/21 Semester 1 - Lab 3
 *
 * Your implementation should go in this file.
 */
#include "entry_controller.h"
#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>

void entry_controller_init( entry_controller_t *entry_controller, int loading_bays ) {
    for (int i = 0; i < 5000; i++) {
        entry_controller->locks[i] = malloc(sizeof(sem_t));
        if (entry_controller->locks[i] == NULL) {
            fprintf(stderr, "Error allocating memory for semaphores.\n");
            exit(1);
        }
        sem_init(entry_controller->locks[i], 0, 0);
    }
    entry_controller->queue_head = 0;
    entry_controller->queue_tail = 0;
    entry_controller->no_of_bays = loading_bays;
    entry_controller->occupied_bays = 0;
    entry_controller->mutex = malloc(sizeof(sem_t));
    if (entry_controller->mutex == NULL) {
        fprintf(stderr, "Error allocating memory for semaphores.\n");
        exit(1);
    }
    sem_init(entry_controller->mutex, 0, 1);
}

void entry_controller_wait( entry_controller_t *entry_controller ) {
    int turn = -1;
    sem_wait(entry_controller->mutex);
    if (entry_controller->occupied_bays < entry_controller->no_of_bays) {
        entry_controller->occupied_bays++;
        sem_post(entry_controller->mutex);
    } else {
        turn = entry_controller->queue_tail++;
        while (1) {
            sem_wait(entry_controller->locks[turn]);
            sem_wait(entry_controller->mutex);
            if (turn == entry_controller->queue_head) {
                entry_controller->queue_head++;
                break;
            } else {
                sem_post(entry_controller->mutex);
                sem_post(entry_controller->locks[turn]);
            }
        }
    }
}

void entry_controller_post( entry_controller_t *entry_controller ) {
    sem_wait(entry_controller->mutex);
    sem_post(entry_controller->locks[entry_controller->queue_head++]);
    sem_post(entry_controller->mutex);
}

void entry_controller_destroy( entry_controller_t *entry_controller ) {

}

