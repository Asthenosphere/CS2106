#!/bin/bash

####################
# Lab 1 Exercise 5
# Name: Wang Luo
# Student No: A0180092L
# Lab Group: 09
####################

echo "Printing system call report"

# compile file
gcc -std=c99 pid_checker.c -o ex5

# use strace to get report
strace -c ./ex5
