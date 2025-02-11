#include "common.h"

#define COLOR_RESET "\033[0m"
#define COLOR_EXECUTABLE "\033[1;32m" // Bold Green
#define COLOR_FILE "\033[1;37m"       // Bold White
#define COLOR_DIRECTORY "\033[1;34m"  // Bold Blue

int alphasort(const struct dirent **a, const struct dirent **b) {
    return strcasecmp((*a)->d_name, (*b)->d_name);
}

void print_file_info(const char *path, const struct dirent *entry, int show_hidden, int long_format) {
    struct stat file_stat;
    char full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

    if (stat(full_path, &file_stat) == -1) {
        perror("stat");
        return;
    }

    if (!show_hidden && entry->d_name[0] == '.') {
        return;
    }

    if (long_format) {
        char time_str[20];
        struct passwd *pw = getpwuid(file_stat.st_uid);
        struct group *gr = getgrgid(file_stat.st_gid);
        strftime(time_str, sizeof(time_str), "%b %d %H:%M", localtime(&file_stat.st_mtime));

        printf("%c%s%s%s%s%s%s%s%s%s %ld %s %s %ld %s ",
               S_ISDIR(file_stat.st_mode) ? 'd' : '-',
               (file_stat.st_mode & S_IRUSR) ? "r" : "-",
               (file_stat.st_mode & S_IWUSR) ? "w" : "-",
               (file_stat.st_mode & S_IXUSR) ? "x" : "-",
               (file_stat.st_mode & S_IRGRP) ? "r" : "-",
               (file_stat.st_mode & S_IWGRP) ? "w" : "-",
               (file_stat.st_mode & S_IXGRP) ? "x" : "-",
               (file_stat.st_mode & S_IROTH) ? "r" : "-",
               (file_stat.st_mode & S_IWOTH) ? "w" : "-",
               (file_stat.st_mode & S_IXOTH) ? "x" : "-",
               file_stat.st_nlink,
               pw->pw_name,
               gr->gr_name,
               file_stat.st_size,
               time_str);
    }

    if (S_ISDIR(file_stat.st_mode)) {
        printf(COLOR_DIRECTORY "%s" COLOR_RESET "\n", entry->d_name);
    } else if (file_stat.st_mode & S_IXUSR) {
        printf(COLOR_EXECUTABLE "%s" COLOR_RESET "\n", entry->d_name);
    } else {
        printf(COLOR_FILE "%s" COLOR_RESET "\n", entry->d_name);
    }
}

void reveal(char *command) {
    char *args[100];
    int i = 0;
    args[i] = strtok(command, " ");
    while (args[i] != NULL) {
        args[++i] = strtok(NULL, " ");
    }

    int show_hidden = 0;
    int long_format = 0;
    char *path = ".";
    for (int j = 1; j < i; j++) {
        if (args[j][0] == '-') {
            for (int k = 1; args[j][k] != '\0'; k++) {
                if (args[j][k] == 'a') {
                    show_hidden = 1;
                } else if (args[j][k] == 'l') {
                    long_format = 1;
                }
            }
        } else {
            path = args[j];
        }
    }
    
    // printf("%s\n", path);


    if (strncmp(path, "~", 1) == 0) {
        char temp[PATH_MAX];
        strcpy(temp, home_dir);
        strcat(temp, path + 1);
        path = temp;
    } else if (strcmp(path, "-") == 0) {
        path = prev_dir;
    }

    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    struct dirent **namelist;
    int n = scandir(path, &namelist, NULL, alphasort);
    if (n < 0) {
        perror("scandir");
        return;
    }

    if(long_format){
        long total_size = 0;
        for (int j = 0; j < n; j++) {
            entry = namelist[j];
            if (!show_hidden && entry->d_name[0] == '.') {
                continue; // Skip hidden files
            }
            char full_path[PATH_MAX + sizeof(entry->d_name) + 1];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
            struct stat file_stat;
            if (stat(full_path, &file_stat) == 0) {
                total_size += file_stat.st_blocks;
            }
        }
        printf("total %ld\n", total_size / 2); // Convert blocks to KB
    }

    for (int j = 0; j < n; j++) {
        entry = namelist[j];
        print_file_info(path, entry, show_hidden, long_format);
        free(entry);
    }
    free(namelist);

    closedir(dir);
}