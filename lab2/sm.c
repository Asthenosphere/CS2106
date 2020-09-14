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
#include <sys/errno.h>
#include "sm.h"

#define READ_END 0
#define WRITE_END 1

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
    int l_pipe[2], r_pipe[2];

    if (pipe(l_pipe) == -1) {
        fprintf(stderr, "Error opening left pipe.");
        exit(1);
    }

    if (pipe(r_pipe) == -1) {
        printf("Error opening right pipe.");
        exit(1);
    }

    int isExit = 0;
    while (!isExit) {
        while (processes[end]) {
            end++;
        }
        end++;

        int pid = fork();

        if (pid == 0) {
            if (start == 0) {
                fprintf(stderr, "76\n");
                dup2(l_pipe[WRITE_END], 1);
                close(l_pipe[READ_END]);
                close(l_pipe[WRITE_END]);
                close(r_pipe[READ_END]);
                close(r_pipe[WRITE_END]);
            } else if (!processes[end]) {
                fprintf(stderr, "83\n");
                close(l_pipe[READ_END]);
                close(l_pipe[WRITE_END]);
                close(r_pipe[READ_END]);
                close(r_pipe[WRITE_END]);
            } else {
                fprintf(stderr, "89\n");
                dup2(r_pipe[READ_END], 0);
                close(l_pipe[READ_END]);
                close(l_pipe[WRITE_END]);
                close(r_pipe[READ_END]);
                close(r_pipe[WRITE_END]);
            }
            // fprintf(stderr, "Command: %s\n", processes[start]);
            execvp(processes[start], (char **) processes + start);
            fprintf(stderr, "Error executing the command.");
            exit(1);
        } else {
            list[count].pid = pid;
            list[count].running = true;
            char * ptr = (char *)malloc(sizeof(char *));
            strcpy(ptr, processes[start]);
            list[count].path = ptr;
            // fprintf(stderr, "95: %s\n", processes[start]);
            if (!processes[end]) {
                terminal_commands[count] = 1;
            }
            count++;
            /*
            for (int i = 0; i < 32; i++) {
                if (list[i].pid) {
                    fprintf(stderr, "%d. %s (PID %ld): %s - Terminal: %d\n", i, list[i].path, (long) list[i].pid,
                            list[i].running ? "Running" : "Exited", terminal_commands[i]);
                    fprintf(stderr, "%s\n", processes[start]);
                }
            }
             */
        }

        start = end;
        if (!processes[end]) {
            close(l_pipe[READ_END]);
            close(l_pipe[WRITE_END]);
            close(r_pipe[READ_END]);
            close(r_pipe[WRITE_END]);
        }

        if (!processes[end]) {
            isExit = 1;
        }
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

