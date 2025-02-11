#include "common.h"

// declaring the global variables

char *home_dir = " ";
char *prev_dir = " ";
search_context_t context;
bg_process bg_processes[MAX_BG_PROCESSES];
int bg_process_count = 0;
int fg_proc_time = 0;
char *fg_proc_name = " ";
