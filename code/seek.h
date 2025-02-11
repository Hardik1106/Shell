#ifndef _SEEK_H
#define _SEEK_H

#include "common.h"

typedef struct {
    char *search_target;
    char *start_dir;
    bool only_dirs;
    bool only_files;
    bool execute_flag;
    int match_count;
    char match_path[PATH_MAX];
} search_context_t;

void seek_init(char *command);

#endif