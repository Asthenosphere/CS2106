#!/bin/bash

####################
# Lab 1 Exercise 4
# Name: Wang Luo
# Student No: A0180092L
# Lab Group: 09
####################

# fill the below up
hostname=xcne6
kernel_version=$(uname -r)
process_cnt=$(ps -e --no-heading | wc -l)
user_process_cnt=$(ps -U "$USER" --no-heading | wc -l)
mem_usage=$(free | grep Mem | awk '{$3/$2 * 100.0}')
swap_usage=$(free | grep Swap | awk '{$3/$2 * 100.0}')

echo "Hostname: $hostname"
echo "Linux Kernel Version: $kernel_version"
echo "Total Processes: $process_cnt"
echo "User Processes: $user_process_cnt"
echo "Memory Used (%): $mem_usage"
echo "Swap Used (%): $swap_usage"
