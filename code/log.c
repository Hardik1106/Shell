#include "common.h"

void handle_log(char *command) {
    if(strncmp(command+4, "purge", 3) == 0){
        log_delete();
    } else if(strncmp(command+4, "execute", 7) == 0){
        log_execute(command+12);
    } else{
        log_print();
    }
}

void log_message(char *message) {
    char logpath[PATH_MAX];
    snprintf(logpath, sizeof(logpath), "%s/log.txt", home_dir);

    int file1 = open(logpath, O_CREAT | O_RDWR, 0644);
    if (file1 == -1) {
        perror("open() error");
        return;
    }
    char line[1024*15];
    read(file1, line, sizeof(line)) > 0;
    close(file1);

    int temp=0;
    int i=0;
    char curr[INPUT_MAX_SIZE];
    int c=0;
    while(line[temp] != '\0'){
        if(line[temp] == '\n'){
            i++;
            if(line[temp+1] != '\0'){
                curr[0]='\0';
            }
            c=0;
        } else{
            curr[c] = line[temp];
            curr[c+1] = '\0';
            c++;
        }
        temp++;
    }

    // printf("%s\n", curr);  
    // printf("%d\n", i);

    if(i > 0 && strncmp(curr,message,strlen(message)) == 0){
        return;
    }

    if(i >= 15){
        int file2 = open(logpath, O_CREAT | O_RDWR | O_TRUNC, 0644);
        if (file2 == -1) {
            perror("open() error");
            return;
        }
        if (file2 == -1) {
            perror("fopen() error");
            return;
        }
        int j = 0;
        while(line[j] != '\n'){
            j++;
        }
        j++;
        write(file2, line+j, strlen(line+j));
        close(file2);
    }

    int file3 = open(logpath, O_CREAT | O_RDWR | O_APPEND, 0644);
    if (file3 == -1) {
        perror("fopen() error");
        return;
    }
    write(file3, message, strlen(message));
    char newline = '\n';
    write(file3, &newline, 1);

    close(file3);

    return;
}

void log_print() {
    char logpath[PATH_MAX];
    snprintf(logpath, sizeof(logpath), "%s/log.txt", home_dir);

    int file = open(logpath, O_CREAT | O_RDWR, 0644);
    if (file == -1) {
        perror("fopen() error");
        return;
    }
    char line[INPUT_MAX_SIZE*15] = "";
    read(file, line, sizeof(line));
    if(strlen(line) == 0){
        printf("....No logs found....\n");
        return;
    }
    printf("%s", line);
    close(file);
    return;
}

void log_delete() {
    char logpath[PATH_MAX];
    snprintf(logpath, sizeof(logpath), "%s/log.txt", home_dir);

    int file = open(logpath, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (file == -1) {
        perror("fopen() error");
        return;
    }
    close(file);
}

void log_execute(char *command) {
    int line_number = atoi(command);
    char logpath[PATH_MAX];
    snprintf(logpath, sizeof(logpath), "%s/log.txt", home_dir);

    int file = open(logpath, O_CREAT | O_RDWR, 0644);
    if (file == -1) {
        perror("fopen() error");
        return;
    }
    char line[INPUT_MAX_SIZE*15];
    read(file, line, sizeof(line));

    int temp=0;
    int currsize=0;
    while(line[temp] != '\0'){
        if(line[temp] == '\n'){
            currsize++;
        }
        temp++;
    }
    if(line_number > currsize){
        printf("Invalid line number\n");
        return;
    }

    line_number = currsize-line_number+1;

    char curr[INPUT_MAX_SIZE];
    int c=0;
    int i=0;
    while(line[i] != '\0'){
        if(line[i] == '\n'){
            line_number--;
            c=0;
        } else {
            curr[c] = line[i];
            curr[++c] = '\0';
        }
        if(line_number == 0){
            break;
        }
        i++;
    }
    printf("Executing: %s\n", curr);
    log_message(curr);
    process_input(curr);
    close(file);
}