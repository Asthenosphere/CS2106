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
#include <signal.h>
#include <fcntl.h>
#include "sm.h"

#define READ_END 0
#define WRITE_END 1

sm_status_t list[32];
int terminal_commands[32];
int logged_processes[32];
int processes_count;
int services_count;

void log_helper(void) {
    char filename[14] = "service";
    if (services_count >= 10) {
        filename[7] = services_count % 10 + '0';
        filename[8] = services_count / 10 + '0';
        filename[9] = 0;
    } else {
        filename[7] = services_count + '0';
        filename[8] = 0;
    }
    strcat(filename, ".log");
    int out = open(filename, O_RDWR | O_APPEND | O_CREAT, 0600);
    if (out == -1) {
        fprintf(stderr, "%s\n", "Cannot open log file");
        exit(1);
    }

    if (dup2(out, fileno(stdout)) != fileno(stdout) ||
        dup2(out, fileno(stderr)) != fileno(stderr)) {
        fprintf(stderr, "%s\n", "Cannot redirect stdout or stderr to file");
    }
    close(out);
    fflush(stdout);
    fflush(stderr);
}

void start_helper(const char * processes[], int is_log) {
    int start = 0, end = 0;

    int l_pipe[2], r_pipe[2];
    if (pipe(l_pipe) == -1) {
        fprintf(stderr, "Error opening left pipe");
        exit(1);
    }

    if (pipe(r_pipe) == -1) {
        fprintf(stderr, "Error opening right pipe");
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
                if (!processes[end]) {
                    if (is_log) {
                        log_helper();
                    }
                } else {
                    dup2(l_pipe[WRITE_END], 1);
                }
                close(l_pipe[READ_END]);
                close(l_pipe[WRITE_END]);
                close(r_pipe[READ_END]);
                close(r_pipe[WRITE_END]);
            } else if (!processes[end]) {
                dup2(r_pipe[READ_END], 0);
                if (is_log) {
                    log_helper();
                }
                close(l_pipe[READ_END]);
                close(l_pipe[WRITE_END]);
                close(r_pipe[READ_END]);
                close(r_pipe[WRITE_END]);
            } else {
                dup2(l_pipe[READ_END], 0);
                dup2(r_pipe[WRITE_END], 1);
                close(l_pipe[READ_END]);
                close(l_pipe[WRITE_END]);
                close(r_pipe[READ_END]);
                close(r_pipe[WRITE_END]);
            }
            execvp(processes[start], (char **) processes + start);
            fprintf(stderr, "Error executing the command： %s\n", processes[start]);
            exit(1);
        } else {
            list[processes_count].pid = pid;
            list[processes_count].running = true;
            char * ptr = (char *)malloc(sizeof(char *));
            strcpy(ptr, processes[start]);
            list[processes_count].path = ptr;
            if (!processes[end]) {
                terminal_commands[processes_count] = 1;
                if (is_log) {
                    logged_processes[services_count] = 1;
                }
                services_count++;
            }
            processes_count++;
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


// Use this function to any initialisation if you need to.
void sm_init(void) {
    processes_count = 0;
    services_count = 0;
    for (int i = 0; i < 32; i++) {
        list[i].pid = 0;
        list[i].running = false;
        list[i].path = NULL;
        terminal_commands[i] = 0;
        logged_processes[i] = 0;
    }
}

// Use this function to do any cleanup of resources.
void sm_free(void) {
}

// Exercise 1a/2: start services
void sm_start(const char *processes[]) {
    start_helper(processes, 0);
}

// Exercise 1b: print service status
size_t sm_status(sm_status_t statuses[]) {
    int size = 0;
    for (int i = 0; i < processes_count; i++) {
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
    int processes_count = (int) index;
    int left, right;
    for (right = 0; right < 32; right++) {
        if (terminal_commands[right]) {
            processes_count--;
        }
        if (processes_count < 0) {
            break;
        }
    }
    left = right;
    while (--left >= 0 && !terminal_commands[left]) {
        continue;
    }
    if (terminal_commands[left] || left < 0) {
        left++;
    }
    int wait_status;
    int kill_status;
    for (int i = left; i <= right; i++) {
        int res = waitpid(list[i].pid, &wait_status, WNOHANG);
        if (res != 0) {
            continue;
        } else {
            kill_status = kill(list[i].pid, SIGTERM);
            if (kill_status == -1) {
                fprintf(stderr, "Error killing the process\n");
                exit(1);
            }
        }
    }
    int status;
    for (int i = left; i <= right; i++) {
        waitpid(list[i].pid, &status, 0);
    }
}

void sm_wait(size_t index) {
    int processes_count = (int) index;
    int left, right;
    for (right = 0; right < 32; right++) {
        if (terminal_commands[right]) {
            processes_count--;
        }
        if (processes_count < 0) {
            break;
        }
    }
    left = right;
    while (--left >= 0 && !terminal_commands[left]) {
        continue;
    }
    if (terminal_commands[left] || left < 0) {
        left++;
    }
    int status;
    for (int i = left; i <= right; i++) {
        waitpid(list[i].pid, &status, 0);
    }
}

void sm_shutdown(void) {
    int num_services = 0;
    for (int i = 0; i < processes_count; i++) {
        if (terminal_commands[i]) {
            num_services++;
        }
    }
    for (int i = 0; i < num_services; i++) {
        sm_stop(i);
    }
}

// Exercise 4: start with output redirection
void sm_startlog(const char *processes[]) {
    start_helper(processes, 1);
}

// Exercise 5: show log file
void sm_showlog(size_t index) {
    if (logged_processes[index]) {
        FILE * fptr;
        char c;
        char filename[14] = "service";
        if (index >= 10) {
            filename[7] = index / 10 + '0';
            filename[8] = index % 10 + '0';
            filename[9] = 0;
        } else {
            filename[7] = index + '0';
            filename[8] = 0;
        }
        strcat(filename, ".log");

        fptr = fopen(filename, "r");
        if (!fptr) {
            fprintf(stderr, "Cannot open log file: %s\n", filename);
            exit(1);
        }
        c = fgetc(fptr);
        while (c != EOF) {
            printf("%c", c);
            c = fgetc(fptr);
        }
        fclose(fptr);
    } else {
        printf("service has no log file\n");
    }
}

