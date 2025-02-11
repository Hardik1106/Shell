#include "common.h"

void sigchld_handler(int signum, siginfo_t *info, void *ptr) {
    handle_background_processes();
}

int main() {
    // Get the home directory
    char temp[PATH_MAX];
    if (getcwd(temp, sizeof(temp)) == NULL) {
        perror("getcwd() error");
        return 1;
    }
    home_dir = malloc(strlen(temp) + 1);
    strcpy(home_dir, temp);
    prev_dir = malloc(PATH_MAX);

    fg_proc_name = malloc(INPUT_MAX_SIZE);

    // Set the signal handler for SIGCHLD
    struct sigaction sa;
    sa.sa_sigaction = sigchld_handler;
    sa.sa_flags = SA_SIGINFO;
    if(sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction() error");
        return 1;
    }

    while (1){
        // Display the prompt
        display_prompt();

        // Read the command
        char command[INPUT_MAX_SIZE];
        if (fgets(command, sizeof(command), stdin) == NULL) {
            // perror("fgets() error , exiting...");
            continue;
        }
        command[strcspn(command, "\n")] = 0; // Remove newline character
        strcpy(command,trim_whitespace(command));

        // Process the command accordingly 
        if(strcmp(command,"exit")==0 || strcmp(command,"quit")==0 || strcmp(command,"q")==0){
            break;
        } else {
            bool flag = 0;
            for(int i=0;i<strlen(command)-2;i++){
                if(strncmp(command+i,"log",3)==0){
                    flag = 1;
                    break;
                }
            }
            if(!flag){
                log_message(command);
            }
            process_input(command);
        }
    }

    free(fg_proc_name);
    free(home_dir);
    free(prev_dir);
    return 0;
}