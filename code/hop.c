#include "common.h"

char* makenice(char *command) {
    if(strncmp(command, "../", 3) == 0 && strlen(command) > 3){
        command[2] = ' ';
    } else if(strncmp(command, "./", 2) == 0 && strlen(command) > 2){
        command[1] = ' ';
    } else if(strncmp(command, "~/", 2) == 0 && strlen(command) > 2){
        command[1] = ' ';
    } else if(strncmp(command, "-/", 2) == 0 && strlen(command) > 2){
        command[1] = ' ';
    }   
    strcpy(command,trim_whitespace(command));
    return command;
}

void hop(char* command) {
    command += 3;
    strcpy(command,trim_whitespace(command));
    strcpy(command,makenice(command));

    char *args[100];
    int i = 0;
    char *token = strtok(command, " ");
    while (token != NULL && i < 100) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }

    for (int j = 0; j < i; j++) {
        char *path = args[j];
        char temp[PATH_MAX];
        if (strncmp(path, "~",1) == 0) {
            strcpy(temp, home_dir);
        } else if (strncmp(path, "..",2) == 0) {
            strcpy(temp, "..");
        } else if (strncmp(path, ".",1) == 0) {
            strcpy(temp, ".");
        } else if (strncmp(path, "-",1) == 0) {
            if(strcmp(prev_dir, "") == 0) {
                printf("No previous directory found\n");
                return;
            }
            strcpy(temp, prev_dir);
        } else {
            strcpy(temp, path);
        }

        // Save current directory as previous directory
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        strcpy(prev_dir, cwd);

        if (chdir(temp) != 0) {
            perror("chdir() error");
            return;
        }

        // Print the full path of the current working directory
        if (getcwd(temp, sizeof(temp)) != NULL) {
            printf("%s\n", temp);
        } else {
            perror("getcwd() error");
        }
    }
    return;
}