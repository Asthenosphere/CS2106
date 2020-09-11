/**
 * CS2106 AY 20/21 Semester 1 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "sm.h"

sm_status_t list[32];
int terminal_commands[32];
int count;

// Use this function to any initialisation if you need to.
void sm_init(void) {
    count = 0;
    for (int i = 0; i < 32; i++) {
        list[i].pid = 0;
        list[i].running = false;
        list[i].path = NULL;
        terminal_commands[i] = 0;
    }
}

// Use this function to do any cleanup of resources.
void sm_free(void) {
}

// Exercise 1a/2: start services
void sm_start(const char *processes[]) {
    int start = 0, end = 0;
    while (1) {
        while (processes[end]) {
            end++;
        }

        int pid;
        pid = fork();

        if (pid == 0) {
            execv(processes[start], (char **) processes + start);
        } else {
            list[count].pid = pid;
            list[count].running = true;
            char * ptr = (char *)malloc(sizeof(char *));
            strcpy(ptr, processes[start]);
            list[count].path = ptr;
            if (!processes[end + 1]) {
                terminal_commands[count] = 1;
            }
            count++;
        }

        if (!processes[++end]) {
            break;
        }

        start = end;
    }
}

// Exercise 1b: print service status
size_t sm_status(sm_status_t statuses[]) {
    int size = 0;
    for (int i = 0; i < count; i++) {
        if (terminal_commands[i]) {
            int status;
            pid_t result = waitpid(list[i].pid, &status, WNOHANG);
            statuses[size].pid = list[i].pid;
            statuses[size].path = list[i].path;
            if (result == 0) {
                statuses[size].running = true;
            } else {
                statuses[i].running = false;
            }
            size++;
        }
    }
    return size;
}

// Exercise 3: stop service, wait on service, and shutdown
void sm_stop(size_t index) {
}

void sm_wait(size_t index) {
}

void sm_shutdown(void) {
}

// Exercise 4: start with output redirection
void sm_startlog(const char *processes[]) {
}

// Exercise 5: show log file
void sm_showlog(size_t index) {
}
