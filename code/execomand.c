#include "common.h"

void handle_background_processes() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        char *curr_proc = NULL;
        for (int i = 0; i < bg_process_count; i++) {
            if (bg_processes[i].pid == pid) {
                curr_proc = bg_processes[i].command;
                for (int j = i; j < bg_process_count - 1; j++) {
                    bg_processes[j] = bg_processes[j + 1];
                }
                bg_process_count--;
                break;
            }
        }
        if (WIFEXITED(status)) {
            if(WEXITSTATUS(status) == 0){
                printf("\nBackground process %s with pid: %d exited normally\n", curr_proc, pid);
            } else {
                printf("\nBackground process %s with pid: %d exited with status %d\n", curr_proc, pid, WEXITSTATUS(status));
            }
        } else if (WIFSIGNALED(status)) {
            printf("\nBackground process %s with pid: %d was terminated by signal %d\n", curr_proc, pid, WTERMSIG(status));
        }
        free(curr_proc);
    }
}

void execute_command(char *command, bool is_background) {
    // custom commands
    strcpy(command, trim_whitespace(command));

    if (strncmp(command, "hop", 3) == 0 || strncmp(command, "cd", 2) == 0) {
        hop(command);
        return;
    }
    if(strncmp(command, "reveal", 6) == 0 && (strncmp(command+6,"\0",1) == 0 || strncmp(command+6," ",1) == 0)){
        reveal(command);
        return;
    }
    if(strncmp(command, "log", 3) == 0 && (strncmp(command+3,"\0",1) == 0 || strncmp(command+3," ",1) == 0)){
        handle_log(command);
        return;
    }
    if(strncmp(command, "proclore", 8) == 0 && (strncmp(command+8,"\0",1) == 0 || strncmp(command+8," ",1) == 0)){
        handle_proclore(command);
        return;
    }
    if(strncmp(command, "seek", 4) == 0 && (strncmp(command+4,"\0",1) == 0 || strncmp(command+4," ",1) == 0)){
        seek_init(command);
        return;
    }

    // system commands
    // printf("%d\n", is_background);


    char *args[100];
    char *rest = command;
    char *token;
    int i = 0;
    while ((token = strtok_r(rest, " ", &rest))) {
        args[i] = token;
        i++;
    }
    args[i] = NULL;

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            printf("Error in executing command: %s\n", args[0]);
        }
        exit(1);
    } else if (pid < 0) {
        // Fork failed
        perror("fork() error");
    } else {
        // Parent process
        if (!is_background) {
            int status;
            time_t start_time, end_time;
            time(&start_time); // Record start time
            waitpid(pid, &status, 0);
            time(&end_time); // Record end time
            double elapsed_time = difftime(end_time, start_time);
            if (elapsed_time > 2) {
                strcpy(fg_proc_name, args[0]);
                fg_proc_time = (int)elapsed_time;
            }
        } else{
            bg_processes[bg_process_count].pid = pid;
            bg_processes[bg_process_count].command = malloc(strlen(args[0]) + 1);
            strcpy(bg_processes[bg_process_count].command, args[0]);
            printf("Background process %s started with pid: %d\n", bg_processes[bg_process_count].command, pid);
            bg_process_count++;
        }
    }
    return;
}