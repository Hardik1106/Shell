#include "common.h"

void handle_proclore(char *command) {
    if (strlen(command) == 8) {
        print_proc_info(getpid());
    } else{
        char *pid_str = command + 9;
        pid_t pid = atoi(pid_str);
        print_proc_info(pid);
        return;
    }
}

void print_proc_info(pid_t pid) {
    char path[PATH_MAX];
    char buffer[INPUT_MAX_SIZE];
    int fd;
    ssize_t len;

    // Print PID
    printf("pid : %d\n", pid);

    // Read process status from /proc/<pid>/stat
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }
    len = read(fd, buffer, sizeof(buffer) - 1);
    if (len == -1) {
        perror("read");
        close(fd);
        return;
    }
    buffer[len] = '\0';
    close(fd);

    // Parse the status and process group from the stat file
    char *token = strtok(buffer, " ");
    for (int i = 0; i < 2; i++) token = strtok(NULL, " "); // Skip pid and comm
    char process_status = token[0];
    for (int i = 0; i < 2; i++) token = strtok(NULL, " "); // Skip ppid and pgrp
    pid_t process_group = atoi(token);

    printf("process status : %c\n", process_status);
    printf("Process Group : %d\n", process_group);

    // Read virtual memory from /proc/<pid>/statm
    snprintf(path, sizeof(path), "/proc/%d/statm", pid);
    fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }
    len = read(fd, buffer, sizeof(buffer) - 1);
    if (len == -1) {
        perror("read");
        close(fd);
        return;
    }
    buffer[len] = '\0';
    close(fd);

    // Parse the virtual memory size
    token = strtok(buffer, " ");
    long virtual_memory = atol(token);
    printf("Virtual memory : %ld\n", virtual_memory);

    // Read executable path from /proc/<pid>/exe
    snprintf(path, sizeof(path), "/proc/%d/exe", pid);
    len = readlink(path, buffer, sizeof(buffer) - 1);
    if (len == -1) {
        perror("readlink");
        return;
    }
    buffer[len] = '\0';

    printf("executable path : %s\n", buffer);
    return;
}