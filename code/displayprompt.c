#include "common.h"

// Define ANSI escape codes for colors
#define COLOR_RESET "\033[0m"
#define COLOR_SYSTEM "\033[1;34m"   // Bold Blue
#define COLOR_USERNAME "\033[38;5;214m" // Orange

void display_prompt() {
    char cwd[PATH_MAX];
    // Get the username and system information
    char *username = getlogin();

    struct utsname sys_info;
    uname(&sys_info);

    // Display the prompt with colors
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        if(fg_proc_time > 2){
            if (strncmp(cwd, home_dir, strlen(home_dir)) == 0) {
                printf("<" COLOR_USERNAME "%s" COLOR_RESET "@" COLOR_SYSTEM "%s" COLOR_RESET ":~%s %s : %ds> ", username, sys_info.nodename, cwd + strlen(home_dir), fg_proc_name, fg_proc_time);
            } else {
                printf("<" COLOR_USERNAME "%s" COLOR_RESET "@" COLOR_SYSTEM "%s" COLOR_RESET ":%s %s : %ds> ", username, sys_info.nodename, cwd, fg_proc_name, fg_proc_time);
            }      
            fg_proc_time = 0;
        } else {
            if (strncmp(cwd, home_dir, strlen(home_dir)) == 0) {
                printf("<" COLOR_USERNAME "%s" COLOR_RESET "@" COLOR_SYSTEM "%s" COLOR_RESET ":~%s> ", username, sys_info.nodename, cwd + strlen(home_dir));
            } else {
                printf("<" COLOR_USERNAME "%s" COLOR_RESET "@" COLOR_SYSTEM "%s" COLOR_RESET ":%s> ", username, sys_info.nodename, cwd);
            }
        }
    } else {
        perror("getcwd() error");
    }
}