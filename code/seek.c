#include "common.h"

char* relativepath(const char *path,const char *start_dir,char* ans) {
    // printf("path: %s\n", path);
    // printf("start_dir: %s\n", start_dir);
    if(strncmp(start_dir,"./",2) == 0) {
        start_dir += 2;
    }
    for(int i = 0; i < strlen(path); i++) {
        if(strncmp(path + i, start_dir, strlen(start_dir)) == 0) {
            strcpy(ans, path + i + strlen(start_dir));
            return ans;
        }
    }
    strcpy(ans, path);
    return ans;
}

int match(const char *path, const struct stat *sb, int typeflag) {
    const char *filename = strrchr(path, '/') ? strrchr(path, '/') + 1 : path;

    if (strcmp(filename, context.search_target) != 0) {
        // Check if the search target is a substring of the filename
        if (!strstr(filename, context.search_target)) {
            return 0;
        }
    }

    if (context.only_dirs && typeflag != FTW_D) {
        return 0;
    }

    if (context.only_files && typeflag != FTW_F) {
        return 0;
    }

    if (context.execute_flag) {
        if (context.match_count == 0) {
            strncpy(context.match_path, path, PATH_MAX);
        } else {
            context.match_path[0] = '\0';
        }
    }

    context.match_count++;

    char ans[PATH_MAX] = "";
    strcpy(ans,relativepath(path,context.start_dir,ans));
    if (typeflag == FTW_D) {
        printf("\033[34m %s \033[0m\n", ans); // Blue for directories
    } else if (typeflag == FTW_F) {
        printf("\033[32m %s \033[0m\n", ans); // Green for files
    }

    return 0;
}

void seek_init(char *command) {
    // skip the first 4 characters
    command += 4;
    strcpy(command, trim_whitespace(command));

    memset(&context, 0, sizeof(context));

    // Parse the command string into an array of arguments
    char *args[100];
    int argc = 0;
    char *token = strtok(command, " ");
    while (token != NULL && argc < 100) {
        args[argc++] = token;
        token = strtok(NULL, " ");
    }

    // for(int i = 0; i < argc; i++) {
    //     printf("%s\n", args[i]);
    // }

    int optind = 0; // Reset getopt's optind
    char flags[] = "dfe";
    int flag_count = strlen(flags);
    for (int i = 0; i < argc; i++) {
        if (args[i][0] == '-') {
            for (int j = 1; j < strlen(args[i]); j++) {
                char flag = args[i][j];
                int valid_flag = 0;
                for (int k = 0; k < flag_count; k++) {
                    if (flag == flags[k]) {
                        valid_flag = 1;
                        break;
                    }
                }
                if (!valid_flag) {
                    printf("Invalid flag: %c\n", flag);
                    return;
                }
                if(flag == 'd') {
                    context.only_dirs = 1;
                } else if (flag == 'f') {
                    context.only_files = 1;
                } else if (flag == 'e') {
                    context.execute_flag = 1;
                }
            }
        } else {
            optind = i;
            break;
        }
    }

    if (context.only_dirs && context.only_files) {
        printf("Invalid flags!");
        return;
    }

    if (optind >= argc) {
        printf("Command not in Correct Format");
        return;
    }

    context.search_target = args[optind++];
    if(optind < argc) {
        context.start_dir = args[optind];   
    } else {
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        context.start_dir = cwd;
    }
    if(context.start_dir[0] == '~') {
        char temp[PATH_MAX];
        strcpy(temp, home_dir);
        strcat(temp, context.start_dir + 1);
        context.start_dir = temp;
    } else if(context.start_dir[0] == '-') {
        context.start_dir = prev_dir;
    }

    strcpy(trim_whitespace(context.search_target), context.search_target);
    // printf("Searching for %s in %s\n", context.search_target, context.start_dir);
    // printf("Only Directories: %d\n", context.only_dirs);
    // printf("Only Files: %d\n", context.only_files);
    // printf("Execute: %d\n", context.execute_flag);

    char target_directory[PATH_MAX];
    if (optind < argc) {
        strcpy(target_directory, context.start_dir);
    } else {
        getcwd(target_directory, sizeof(target_directory));
    }

    if (ftw(target_directory, match, 20) == -1) {
        perror("ftw");
        return;
    }
    

    if (context.match_count == 0) {
        printf("No match found!\n");
    } else if (context.execute_flag && context.match_count == 1) {
        struct stat sb;
        if (stat(context.match_path, &sb) == -1) {
            perror("stat");
            return;
        }
        if (S_ISDIR(sb.st_mode)) {
            if (chdir(context.match_path) == -1) {
                printf("Missing Permissions for task!\n");
                return;
            }
        } else if (S_ISREG(sb.st_mode)) {
            FILE *file = fopen(context.match_path, "r");
            if (!file) {
                printf("Missing Permissions for task!\n");
                return;
            }

            char line[256];
            while (fgets(line, sizeof(line), file)) {
                printf("%s", line);
            }
            printf("\n");

            fclose(file);
        }
    }

    return;
}