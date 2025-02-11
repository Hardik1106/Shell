#ifndef _COMMON_H
#define _COMMON_H



#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <fcntl.h>
#include <ftw.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include "displayprompt.h"
#include "trimextra.h"
#include "execomand.h"
#include "processinput.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "proclore.h"
#include "seek.h"

#define PATH_MAX 4096
#define INPUT_MAX_SIZE 1024
extern char* home_dir;
extern char* prev_dir;
extern search_context_t context;

typedef struct bg_process {
    pid_t pid;
    char *command;
} bg_process;

#define MAX_BG_PROCESSES 100
extern bg_process bg_processes[MAX_BG_PROCESSES];
extern int bg_process_count;

extern int fg_proc_time;
extern char* fg_proc_name;

#endif